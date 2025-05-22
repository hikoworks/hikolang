
#include "token.hpp"
#include "char_category.hpp"
#include <concepts>
#include <string>
#include <string_view>
#include <expected>
#include <cstdint>
#include <cstddef>

namespace hl {

struct parse_token_context {
    std::size_t line_nr = 0;
    std::size_t column_nr = 0;
    std::size_t module_id = 0;

    std::string text = {};

    [[nodiscard]] constexpr token make_token(token::kind_type kind, char c) const noexcept
    {
        return token{kind, module_id, line_nr, column_nr, c};
    }

    constexpr void advance(std::string_view::const_iterator& it, size_t count = 1) noexcept
    {
        for (size_t i = 0; i != count; ++i) {
            auto const c = *it++;

            if (c == '\n') {
                ++line_nr;
                column_nr = 0;
            } else if ((c & 0xc0) == 0x80) {
                // Skip UTF-8 continuation code-unit.
            } else {
                ++column_nr;
            }
        }
    }
};


template<std::invocable<hl::token> F>
constexpr std::expected<void, std::string> tokenize(
    parse_token_context& context,
    std::string_view::const_iterator& it,
    std::string_view::const_iterator end,
    size_t module_id,
    F&& f)
{
    auto found_cr = false;

    while (it != end) {
        auto const start_it = it;

        // Decode UTF-8 code-point, to deal with multi-byte line endings,
        // separators and brackets.
        auto cp = char32_t{};
        if (auto const optional_cp = decode_utf8_code_point(it, end)) {
            cp = *optional_cp;
        } else {
            return std::unexpected{context.make_error(optional_cp.error())};
        }

        if (found_cr and cp != '\n') {
            // Treat a solo CR as a LF. Solo CR used to be old style MacOS line
            // endings. Insert a LF token before the next token.
            found_cr = false;
            f(context.make_token(token::line_feed, '\r'));
        }

        if (cp == '\r') {
            // Found a CR, treat the same as horizontal space. But remember.
            found_cr = true;
            context.advance_horizontal();

        } else if (is_vertical_space(cp)) {
            f(context.make_token(token::line_feed, cp));
            context.advance_vertical();

        } else if (is_horizontal_space(cp)) {
            context.advance_horizontal();

        } else if (is_bracket(cp)) {
            f(context.make_token(token::bracket, cp));
            context.advance_horizontal();

        } else if (is_separator(cp)) {
            f(context.make_token(token::seperator, cp));
            context.advance_horizontal();

        } else if (is_digit(cp)) {
            it = start_it;
            if (auto const optional_token = parse_number(context, it, end)) {
                f(*optional_token);
            } else {
                return std::unexpected{optional_token.error()};
            }

        } else if (cp == '"' or cp == '\'' or cp == '`') {
            it = start_it;
            if (auto const optional_token = parse_string(context, it, end)) {
                f(*optional_token);
            } else {
                return std::unexpected{optional_token.error()};
            }
        
        } else if (is_identifier_start(cp) and cp != 'r') {
            it = start_it;
            if (auto const optional_token = parse_identifier(context, it, end)) {
                f(*optional_token);
            } else {
                return std::unexpected{optional_token.error()};
            }

        } else if (is_pattern_syntax(cp)) {
            it = start_it;
            if (auto const optional_token = parse_operator(context, it, end)) {
                f(*optional_token);
            } else {
                return std::unexpected{optional_token.error()};
            }

        } else {
            // For more complex tokens, we need to look ahead.
            auto cp_next = char32_t{};
            if (it != end) {
                auto tmp_it = it;
                if (auto const optional_cp = decode_utf8_code_point(tmp_it, end)) {
                    cp_next = *optional_cp;
                } else {
                    return std::unexpected{context.make_error(optional_cp.error())};
                }
            }

            if (cp == '.') {
                if (is_digit(cp_next)) {
                    // This is a float literal.
                    it = start_it;
                    if (auto const optional_token = parse_float(context, it, end)) {
                        f(*optional_token);
                    } else {
                        return std::unexpected{optional_token.error()};
                    }

                } else {
                    f(context.make_token(token::_operator, cp));
                    context.advance_horizontal();
                }

            } else if (cp == 'r') {
                if (cp_next == '"' or cp_next == '\'' or cp_next == '`') {
                    // This is a raw string literal.
                    it = start_it;
                    if (auto const optional_token = parse_raw_string(context, it, end)) {
                        f(*optional_token);
                    } else {
                        return std::unexpected{optional_token.error()};
                    }
                } else {
                    // This is a identifier.
                    it = start_it;
                    if (auto const optional_token = parse_identifier(context, it, end)) {
                        f(*optional_token);
                    } else {
                        return std::unexpected{optional_token.error()};
                    }
                }
            }
        }
    }

}

} // namespace hl
