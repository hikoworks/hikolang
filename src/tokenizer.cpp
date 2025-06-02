
#include "tokenizer.hpp"
#include "utf8.hpp"
#include "module.hpp"
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <array>
#include <algorithm>
#include <format>
#include <expected>

namespace hl {

tokenizer::tokenizer(size_t module_id, std::string_view module_text) noexcept :
    _lookahead(), _ptr(module_text.data()), _end(_ptr + module_text.size()), _module_id(module_id), _line_nr(0), _column_nr(0)
{
    assert(_ptr != nullptr);
    assert(_end != nullptr);
    assert(_end >= _ptr);

    // Fill the lookahead buffer.
    _lookahead.decode_utf8(_ptr, _end);
}

[[nodiscard]] std::expected<void, std::string> tokenizer::tokenize(delegate_type& delegate)
{
    assert(_line_nr == 0);
    assert(_column_nr == 0);
    assert(_ptr != nullptr);
    assert(_end != nullptr);

    auto found_cr = false;

    while (not end_of_file()) {
        auto const cp = _lookahead[0].cp;
        auto const cp_ptr = _lookahead[0].start;
        auto const cp2 = _lookahead[1].cp;
        auto const cp3 = _lookahead[2].cp;

        if (found_cr and cp != '\n') {
            // Treat a solo CR as a LF. Solo CR used to be old style MacOS line
            // endings. Insert a LF token before the next token.
            found_cr = false;
            delegate.on_token(make_token(token::line_feed, '\r'));
        }

        if (cp == '\r') {
            // Found a CR, treat the same as horizontal space. But remember.
            found_cr = true;
            advance();
            continue;
        }

        if (is_vertical_space(cp)) {
            delegate.on_token(make_token(token::line_feed, '\n'));
            advance();
            continue;
        }
        
        if (is_horizontal_space(cp) or is_ignoreable(cp)) {
            advance();
            continue;
        }

        if (is_bracket(cp)) {
            delegate.on_token(make_token(token::bracket, cp));
            advance();
            continue;
        }

        if (is_separator(cp)) {
            delegate.on_token(make_token(token::seperator, cp));
            advance();
            continue;
        }

        if (auto const token = parse_line_comment(); token.has_error()) {
            return std::unexpected{token.error()};
        } else if (token.has_value()) {
            delegate.on_token(*token);
            continue;
        }

        if (auto const token = parse_block_comment(); token.has_error()) {
            return std::unexpected{token.error()};
        } else if (token.has_value()) {
            delegate.on_token(*token);
            continue;
        }

        if (cp == '*' and cp2 == '/') {
            return make_error("Unexpected end of comment; found '*/' without a matching '/*'.");
        }

        if (auto const token = parse_number(); token.has_error()) {
            return std::unexpected{token.error()};
        } else if (token.has_value()) {
            delegate.on_token(*token);
            continue;
        }

        if (auto const token = parse_string(); token.has_error()) {
            return std::unexpected{token.error()};
        } else if (token.has_value()) {
            delegate.on_token(*token);
            continue;
        }

        if (parse_line_directive()) {
            // A line directive only affects the tokenizer state, it does not
            // produce a token.
            continue;
        }

        if (auto const token = parse_positional_argument(); token.has_error()) {
            return std::unexpected{token.error()};
        } else if (token.has_value()) {
            delegate.on_token(*token);
            continue;
        }

        if (cp == '$' and cp2 == '#') {
            delegate.on_token(make_token(token::positional_argument, '#'));
            advance(); // Skip the '$'
            advance(); // Skip the '#'
            continue;
        }

        if (auto const token = parse_identifier(); token.has_error()) {
            return std::unexpected{token.error()};
        } else if (token.has_value()) {
            delegate.on_token(*token);
            continue;
        }

        if (auto const token = parse_operator(); token.has_error()) {
            return std::unexpected{token.error()};
        } else if (token.has_value()) {
            delegate.on_token(*token);
            continue;
        }

        return make_unexpected_character_error(cp, cp_ptr);
    }

    delegate.on_eof();
    return {};
}

[[nodiscard]] bool tokenizer::end_of_file() const noexcept
{
    auto const empty = _lookahead.empty();
    assert(not empty or _ptr == _end);
    return empty;
}

void tokenizer::advance() noexcept
{
    auto const cp = _lookahead[0].cp;
    _lookahead.pop_front();
    _lookahead.decode_utf8(_ptr, _end);

    if (is_vertical_space(cp)) {
        ++_line_nr;
        _column_nr = 0;
    } else {
        ++_column_nr;
    }
}

[[nodiscard]] std::unexpected<std::string> tokenizer::make_unexpected_character_error(char32_t cp, char const* start) const
{
    switch (cp) {
    case std::to_underlying(unicode_error::continuation_byte):
        return make_error("Invalid UTF-8 sequence; found a lone continuation byte {}.", display_utf8_sequence(start, _end));

    case std::to_underlying(unicode_error::missing_continuation_byte):
        return make_error(
            "Invalid UTF-8 sequence; found a non-continuation byte in a multi-byte sequence {}.",
            display_utf8_sequence(start, _end));

    case std::to_underlying(unicode_error::buffer_overrun):
        return make_error(
            "Invalid UTF-8 sequence; multi-byte sequence continues beyond end of buffer {}.", display_utf8_sequence(start, _end));

    case std::to_underlying(unicode_error::overlong_encoding):
        return make_error("Invalid UTF-8 sequence; overlong encoding {}.", display_utf8_sequence(start, _end));

    case std::to_underlying(unicode_error::out_of_range):
        return make_error("Invalid UTF-8 sequence; code-point out of range {}.", display_utf8_sequence(start, _end));

    case std::to_underlying(unicode_error::surrogate):
        return make_error("Invalid UTF-8 sequence; surrogate value {}.", display_utf8_sequence(start, _end));

    default:
        auto optional_cp_str = encode_utf8_code_point(cp);
        assert(optional_cp_str.has_value());
        return make_error("Unexpected character: '{}' U+{:06x}.", *optional_cp_str, static_cast<uint32_t>(cp));
    }
}




[[nodiscard]] std::expected<void, std::string>
tokenize(size_t module_id, std::string_view module_text, tokenize_delegate& delegate)
{
    auto context = tokenizer{module_id, module_text};

    struct simple_delegate_type : tokenizer::delegate_type {
        tokenize_delegate& delegate;

        explicit simple_delegate_type(tokenize_delegate& delegate) noexcept : delegate(delegate) {}

        void on_token(token const& t) override
        {
            delegate.on_token(t);
        }

        void on_eof() override
        {
            delegate.on_eof();
        }
    };

    auto simple_delegate = simple_delegate_type{delegate};
    return context.tokenize(simple_delegate);
}

} // namespace hl
