
#include "tokenizer.hpp"
#include "utf8.hpp"
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

[[nodiscard]] std::expected<token, std::string> tokenizer::parse_string()
{
    enum class state_type {
        idle,
        escape,
        escape_u,
        escape_N
    };

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

    auto state = state_type::idle;
    auto value_length = uint64_t{0};
    auto value = uint64_t{0};
    auto name = std::string{};
    while (decode_utf8()) {
        auto const cp = _lookahead[0].cp;

        switch (state) {
        case state_type::idle:
            if (cp == quote_char) {
                // End of string literal.
                r.append(quote_char);
                advance();
                return r;

            } else if (cp == '\\') {
                // Escape sequence.
                state = state_type::escape;

            } else {
                r.append(cp);
            }
            break;

        case state_type::escape:
            // clang-format off
            switch (cp) {
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
                return make_error("Invalid escape sequence in string literal.");
            }
            // clang-format on
            break;

        case state_type::escape_u:
            if (cp == '{' and value == 0) {
                // Brace escape sequence, we will read until we find a '}'.
                value_length = std::numeric_limits<uint64_t>::max();

            } else if (cp == '}') {
                // End of brace escape sequence.
                value_length = 1;

            } else if (cp >= '0' and cp <= '9') {
                value <<= 4;
                value |= cp - '0';

            } else if (cp >= 'a' and cp <= 'f') {
                value <<= 4;
                value = cp - 'a' + 10;

            } else if (cp >= 'A' and cp <= 'F') {
                value <<= 4;
                value = cp - 'A' + 10;

            } else {
                // Invalid escape sequence.
                return make_error("Invalid hex-digit in \\u escape sequence.");
            }

            if (--value_length == 0) {
                // We have a complete code-point.
                if (value > 0x10FFFF) {
                    return make_error("Invalid code-point U+{:06x} in \\u escape sequence.", value);
                }
                if (value >= 0xD800 and value <= 0xDFFF) {
                    return make_error("Invalid surrogate code-point U+{:06x} in \\u escape sequence.", value);
                }
                r.append(static_cast<char32_t>(value));
                state = state_type::idle;
            }
            break;

        case state_type::escape_N:
            if (cp == '{') {
                // Start of name escape sequence.
                name.clear();

            } else if (cp == '}') {
                // End of name escape sequence.
                if (name.empty()) {
                    return make_error("Empty name in \\N escape sequence.");
                }

                if (auto new_cp = unicode_name_to_code_point(name); new_cp != std::to_underlying(unicode_error::name_not_found)) {
                    r.append(new_cp);
                } else {
                    return make_error("Could not find unicode name '{}'", name);
                }

                state = state_type::idle;

            } else {
                if ((cp >= 'A' and cp <= 'Z') or (cp >= '0' and cp <= '9') or cp == ' ' or cp == '(' or cp == ')') {
                    // Valid character in name.
                    name.push_back(static_cast<char>(cp));

                } else {
                    // Invalid character in name.
                    return make_error("Invalid character U+{:04x}' in \\N escape sequence.", static_cast<uint32_t>(cp));
                }
            }
            break;

        }

        advance();
    }

    return make_error("Unterminated string literal.");
}

}
