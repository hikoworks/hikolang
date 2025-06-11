
#include "token_parsers.hpp"
#include <format>
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <string>
#include <string_view>
#include <expected>
#include <utility>
#include <limits>

namespace hl {

[[nodiscard]] std::optional<token> parse_string(file_cursor& c)
{
    enum class state_type {
        idle,
        escape,
        escape_u,
        escape_N
    };

    auto const is_raw_string = c[0] == 'r';
    auto const quote_char = is_raw_string ? c[1] : c[0];
    if (quote_char != '\'' and quote_char != '"' and quote_char != '`') {
        return std::nullopt;
    }

    auto r = token{c.location(), token::empty};

    if (is_raw_string) {
        ++c;
    }
    ++c;
    
    if (quote_char == '"') {
        r.kind = token::string_literal;
    } else if (quote_char == '\'') {
        r.kind = token::character_literal;
    } else if (quote_char == '`') {
        r.kind = token::quote_literal;
    } else {
        std::unreachable();
    }

    auto state = state_type::idle;
    auto value_length = uint64_t{0};
    auto value = uint64_t{0};
    auto name = std::string{};
    for (; not c.end_of_file(); ++c) {
        switch (state) {
        case state_type::idle:
            if (c[0] == quote_char) {
                // End of string literal.
                r.last = c.location();
                return r;

            } else if (c[0] == '\\') {
                // Escape sequence.
                state = state_type::escape;

            } else {
                r.append(c[0]);
            }
            break;

        case state_type::escape:
            // clang-format off
            switch (c[0]) {
            case '\'': state = state_type::idle; r.append('\''); break;
            case '"':  state = state_type::idle; r.append('\"'); break;
            case '?':  state = state_type::idle; r.append('\?'); break;
            case '\\': state = state_type::idle; r.append('\\'); break;
            case 'a':  state = state_type::idle; r.append('\a'); break;
            case 'b':  state = state_type::idle; r.append('\b'); break;
            case 'f':  state = state_type::idle; r.append('\f'); break;
            case 'n':  state = state_type::idle; r.append('\n'); break;
            case 'r':  state = state_type::idle; r.append('\r'); break;
            case 't':  state = state_type::idle; r.append('\t'); break;
            case 'v':  state = state_type::idle; r.append('\v'); break;
            case 'u':  state = state_type::escape_u; value_length = 4; value = 0; break;
            case 'U':  state = state_type::escape_u; value_length = 6; value = 0; break;
            case 'N':  state = state_type::escape_N; break;
            default:
                // Invalid escape sequence.
                return r.make_error(c.location(), "Invalid escape sequence in string literal.");
            }
            // clang-format on
            break;

        case state_type::escape_u:
            if (c[0] == '{' and value == 0) {
                // Brace escape sequence, we will read until we find a '}'.
                value_length = std::numeric_limits<uint64_t>::max();

            } else if (c[0] == '}') {
                // End of brace escape sequence.
                value_length = 1;

            } else if (c[0] >= '0' and c[0] <= '9') {
                value <<= 4;
                value |= c[0] - '0';

            } else if (c[0] >= 'a' and c[0] <= 'f') {
                value <<= 4;
                value = c[0] - 'a' + 10;

            } else if (c[0] >= 'A' and c[0] <= 'F') {
                value <<= 4;
                value = c[0] - 'A' + 10;

            } else {
                // Invalid escape sequence.
                return r.make_error(c.location(), "Invalid hex-digit in \\u escape sequence.");
            }

            if (--value_length == 0) {
                // We have a complete code-point.
                if (value > 0x10FFFF) {
                    return r.make_error(c.location(), "Invalid code-point U+{:06x} in \\u escape sequence.", value);
                }
                if (value >= 0xD800 and value <= 0xDFFF) {
                    return r.make_error(c.location(), "Invalid surrogate code-point U+{:06x} in \\u escape sequence.", value);
                }
                r.append(static_cast<char32_t>(value));
                state = state_type::idle;
            }
            break;

        case state_type::escape_N:
            if (c[0] == '{') {
                // Start of name escape sequence.
                name.clear();

            } else if (c[0] == '}') {
                // End of name escape sequence.
                if (name.empty()) {
                    return r.make_error(c.location(), "Empty name in \\N escape sequence.");
                }

                if (auto new_cp = unicode_name_to_code_point(name); new_cp != std::to_underlying(unicode_error::name_not_found)) {
                    r.append(new_cp);
                } else {
                    return r.make_error(c.location(), "Could not find unicode name '{}'", name);
                }

                state = state_type::idle;

            } else {
                if ((c[0] >= 'A' and c[0] <= 'Z') or (c[0] >= '0' and c[0] <= '9') or c[0] == ' ' or c[0] == '(' or c[0] == ')') {
                    // Valid character in name.
                    name.push_back(static_cast<char>(c[0]));

                } else {
                    // Invalid character in name.
                    return r.make_error(c.location(), "Invalid character U+{:04x}' in \\N escape sequence.", static_cast<uint32_t>(c[0]));
                }
            }
            break;

        }
    }

    return r.make_error(c.location(), "Unterminated string literal.");
}

}
