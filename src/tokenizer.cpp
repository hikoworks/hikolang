
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
}

[[nodiscard]] std::expected<void, std::string> tokenizer::tokenize(delegate_type& delegate)
{
    assert(_line_nr == 0);
    assert(_column_nr == 0);
    assert(_ptr != nullptr);
    assert(_end != nullptr);

    auto found_cr = false;

    while (decode_utf8()) {
        auto const cp = _lookahead[0].cp;
        auto const cp_ptr = _lookahead[0].start;
        auto const next_cp = _lookahead[1].cp;
        auto const next_next_cp = _lookahead[2].cp;

        if (found_cr and cp != '\n') {
            // Treat a solo CR as a LF. Solo CR used to be old style MacOS line
            // endings. Insert a LF token before the next token.
            found_cr = false;
            delegate.on_token(make_character_token(token::line_feed, '\r'));
        }

        if (cp == '\r') {
            // Found a CR, treat the same as horizontal space. But remember.
            found_cr = true;
            advance();

        } else if (is_vertical_space(cp)) {
            delegate.on_token(make_character_token(token::line_feed, '\n'));
            advance();

        } else if (is_horizontal_space(cp)) {
            advance();

        } else if (is_ignoreable(cp)) {
            advance();

        } else if (is_bracket(cp)) {
            delegate.on_token(make_character_token(token::bracket, cp));
            advance();

        } else if (is_separator(cp)) {
            delegate.on_token(make_character_token(token::seperator, cp));
            advance();

        } else if (
            is_digit(cp) or (cp == '.' and is_digit(next_cp)) or
            ((cp == '-' or cp == '+') and (is_digit(next_cp) or (next_cp == '.' and is_digit(next_next_cp))))) {
            // `-`, `+`, `.` and `..` are valid operators, distinguish them from numbers.
            if (auto const optional_token = parse_number()) {
                delegate.on_token(*optional_token);
            } else {
                return std::unexpected{optional_token.error()};
            }

        } else if (
            cp == '"' or cp == '\'' or cp == '`' or (cp == 'r' and (next_cp == '"' or next_cp == '\'' or next_cp == '`'))) {
            if (auto const optional_token = parse_string()) {
                delegate.on_token(*optional_token);
            } else {
                return std::unexpected{optional_token.error()};
            }

        } else if (is_identifier_start(cp)) {
            if (auto const optional_token = parse_identifier()) {
                delegate.on_token(*optional_token);
            } else {
                return std::unexpected{optional_token.error()};
            }

        } else if (is_pattern_syntax(cp)) {
            if (auto const optional_token = parse_operator()) {
                delegate.on_token(*optional_token);
            } else {
                return std::unexpected{optional_token.error()};
            }

        } else {
            return make_unexpected_character_error(cp, cp_ptr);
        }
    }

    return {};
}

bool tokenizer::decode_utf8() noexcept
{
    assert(_ptr != nullptr);
    assert(_end != nullptr);
    return _lookahead.decode_utf8(_ptr, _end);
}

char32_t tokenizer::advance() noexcept
{
    auto const cp = _lookahead[0].cp;
    _lookahead.pop_front();

    if (is_vertical_space(cp)) {
        ++_line_nr;
        _column_nr = 0;
    } else {
        ++_column_nr;
    }

    return cp;
}

[[nodiscard]] token tokenizer::make_token(token::kind_type kind) const
{
    return token{_module_id, _line_nr, _column_nr, kind};
}

[[nodiscard]] token tokenizer::make_character_token(token::kind_type kind, char c) const
{
    return token{_module_id, _line_nr, _column_nr, kind, c};
}

[[nodiscard]] token tokenizer::make_character_token(token::kind_type kind, char32_t c) const
{
    return token{_module_id, _line_nr, _column_nr, kind, c};
}

[[nodiscard]] std::unexpected<std::string> tokenizer::make_unexpected_character_error(char32_t cp, char const* start) const
{
    switch (cp) {
    case std::to_underlying(decode_utf8_error::continuation_byte):
        return make_error("Invalid UTF-8 sequence; found a lone continuation byte {}.", display_utf8_sequence(start, _end));

    case std::to_underlying(decode_utf8_error::missing_continuation_byte):
        return make_error(
            "Invalid UTF-8 sequence; found a non-continuation byte in a multi-byte sequence {}.",
            display_utf8_sequence(start, _end));

    case std::to_underlying(decode_utf8_error::buffer_overrun):
        return make_error(
            "Invalid UTF-8 sequence; multi-byte sequence continues beyond end of buffer {}.", display_utf8_sequence(start, _end));

    case std::to_underlying(decode_utf8_error::overlong_encoding):
        return make_error("Invalid UTF-8 sequence; overlong encoding {}.", display_utf8_sequence(start, _end));

    case std::to_underlying(decode_utf8_error::out_of_range):
        return make_error("Invalid UTF-8 sequence; code-point out of range {}.", display_utf8_sequence(start, _end));

    case std::to_underlying(decode_utf8_error::surrogate):
        return make_error("Invalid UTF-8 sequence; surrogate value {}.", display_utf8_sequence(start, _end));

    default:
        auto optional_cp_str = encode_utf8_code_point(cp);
        assert(optional_cp_str.has_value());
        return make_error("Unexpected character: '{}' U+{:06x}.", *optional_cp_str, static_cast<uint32_t>(cp));
    }
}

[[nodiscard]] std::expected<token, std::string> tokenizer::parse_string()
{
    auto r = make_token(token::string_literal);

    auto const is_raw_string = [&] {
        if (_lookahead[0].cp == 'r') {
            advance();
            return true;
        }
        return false;
    }();

    auto const quote_char = _lookahead[0].cp;
    advance();

    auto escape = false;
    while (decode_utf8()) {
        auto const cp = _lookahead[0].cp;

        if (cp == quote_char and not escape) {
            // End of string literal.
            r.append(quote_char);
            advance();
            return r;

        } else if (cp == '\\') {
            // Escape sequence.
            advance();
            escape = true;
        }

        advance();
    }

    return make_error("Unterminated string literal.");
}

[[nodiscard]] std::expected<token, std::string> tokenizer::parse_identifier()
{
    auto r = make_token(token::identifier);
    auto const start_ptr = _lookahead[0].start;

    // Skip the first character, which is guaranteed to be an identifier start.
    advance();

    while (decode_utf8()) {
        auto const cp = _lookahead[0].cp;

        if (not is_identifier_continue(cp) and cp != '_') {
            // End of identifier.
            r.text = std::string_view{start_ptr, _lookahead[0].start};
            return r;
        }

        advance();
    }

    r.text = std::string_view{start_ptr, _end};
    return r;
}

[[nodiscard]] std::expected<token, std::string> tokenizer::parse_operator()
{
    auto r = make_token(token::_operator);
    auto const start_ptr = _lookahead[0].start;

    // Skip the first character, which is guaranteed to be a pattern syntax character.
    advance();

    while (decode_utf8()) {
        if (not is_pattern_syntax(_lookahead[0].cp)) {
            // End of operator.
            r.text = std::string_view{start_ptr, _lookahead[0].start};
            return r;
        }

        advance();
    }

    r.text = std::string_view{start_ptr, _end};
    return r;
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
    };

    auto simple_delegate = simple_delegate_type{delegate};
    return context.tokenize(simple_delegate);
}

} // namespace hl
