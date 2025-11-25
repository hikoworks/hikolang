
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

namespace hk {

[[nodiscard]] token parse_string(char const*& p)
{
    auto const is_raw_string = p[0] == 'r';
    auto const quote_char = is_raw_string ? p[1] : p[0];
    if (quote_char != '\'' and quote_char != '"' and quote_char != '`') {
        return {};
    }

    auto const string_type = [&] {
        switch (quote_char) {
        case '"':
            return is_raw_string ? token::raw_string_literal : token::string_literal;
        case '\'':
            return is_raw_string ? token::raw_character_literal : token::character_literal;
        case '`':
            return is_raw_string ? token::raw_quote_literal : token::quote_literal;
        default:
            std::unreachable();
        }
    }();

    auto r = token{p += is_raw_string ? 2 : 1, string_type};

    auto escape = false;
    for (; p[0] != '\0'; ++p) {
        if (escape) {
            escape = false;
        } else {
            if (p[0] == quote_char) {
                r.set_last(p);
                ++p;
                return r;

            } else if (p[0] == '\\') {
                escape = true;
            }
        }
    }

    return r.make_error(p, token::unterminated_string_literal_error);
}
} // namespace hk
