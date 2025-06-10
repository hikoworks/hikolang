
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

static void simple_tokenize(hl::cursor &c, tokenize_delegate& delegate)
{
    auto found_cr = false;

    while (not cursor.end_of_file()) {
        if (found_cr and c[0] != '\n') {
            // Treat a solo CR as a LF. Solo CR used to be old style MacOS line
            // endings. Insert a LF token before the next token.
            found_cr = false;
            delegate.on_token(make_token(token::line_feed, '\r'));
        }

        if (c[0] == '\r') {
            // Found a CR, treat the same as horizontal space. But remember.
            found_cr = true;
            ++c;
            continue;
        }

        if (is_vertical_space(c[0])) {
            delegate.on_token(make_token(token::line_feed, '\n'));
            ++c;
            continue;
        }
        
        if (is_horizontal_space(c[0]) or is_ignoreable(c[0])) {
            ++c;
            continue;
        }

        if (is_bracket(c[0])) {
            delegate.on_token(make_token(token::bracket, c[0]));
            ++c;
            continue;
        }

        if (is_separator(c[0])) {
            delegate.on_token(make_token(token::seperator, c[0]));
            ++c;
            continue;
        }

        if (auto token = parse_line_comment(c); token.has_error()) {
            return std::unexpected{token.error()};
        } else if (token.has_value()) {
            delegate.on_token(std::move(token).value());
            continue;
        }

        if (auto token = parse_block_comment(c); token.has_error()) {
            return std::unexpected{token.error()};
        } else if (token.has_value()) {
            delegate.on_token(std::move(token).value());
            continue;
        }

        if (c[0] == '*' and c[1] == '/') {
            return make_error("Unexpected end of comment; found '*/' without a matching '/*'.");
        }

        if (auto token = parse_version(c); token.has_error()) {
            return std::unexpected{token.error()};
        } else if (token.has_value()) {
            delegate.on_token(std::move(token).value());
            continue;
        }

        if (auto token = parse_number(c); token.has_error()) {
            return std::unexpected{token.error()};
        } else if (token.has_value()) {
            delegate.on_token(std::move(token).value());
            continue;
        }

        if (auto token = parse_string(c); token.has_error()) {
            return std::unexpected{token.error()};
        } else if (token.has_value()) {
            delegate.on_token(std::move(token).value());
            continue;
        }

        if (parse_line_directive(c)) {
            // A line directive only affects the tokenizer state, it does not
            // produce a token.
            continue;
        }

        if (auto token = parse_positional_argument(c); token.has_error()) {
            return std::unexpected{token.error()};
        } else if (token.has_value()) {
            delegate.on_token(std::move(token).value());
            continue;
        }

        if (cp == '$' and cp2 == '#') {
            delegate.on_token(make_token(token::positional_argument, '#'));
            c += 2; // Skip the '$#'
            continue;
        }

        if (auto token = parse_identifier(c); token.has_error()) {
            return std::unexpected{token.error()};
        } else if (token.has_value()) {
            delegate.on_token(std::move(token).value());
            continue;
        }

        if (auto token = parse_operator(c); token.has_error()) {
            return std::unexpected{token.error()};
        } else if (token.has_value()) {
            delegate.on_token(std::move(token).value());
            continue;
        }

        return make_unexpected_character_error(c);
    }

    delegate.on_token(make_token(token::eof,));
    return {};
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




void tokenize(hl::cursor &cursor, tokenize_delegate& delegate)
{
    struct simple_delegate_type : tokenizer::delegate_type {
        tokenize_delegate& delegate;

        explicit simple_delegate_type(tokenize_delegate& delegate) noexcept : delegate(delegate) {}

        void on_token(token const& t) override
        {
            delegate.on_token(t);
        }
    };

    auto simple_delegate = simple_delegate_type{delegate};
    return simple_tokenize(cursor, simple_delegate);
}

} // namespace hl
