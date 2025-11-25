

#include "token_parsers.hpp"
#include "char_category.hpp"

namespace hk {

[[nodiscard]] token parse_position_arg(char const*& p)
{
    if (p[0] != '$' or not is_digit(p[1])) {
        return {};
    }

    auto r = token{++p, token::position_arg};
    ++p;

    for (; p[0] != '\0'; ++p) {
        if (not is_digit(p[0])) {
            r.set_last(p);
            return r;
        }
    }

    r.set_last(p);
    return r;
}

}
