

#include "token_parsers.hpp"
#include "char_category.hpp"
#include <iterator>

namespace hk {

[[nodiscard]] token parse_context_arg(char const*& p)
{
    if (p[0] != '$') {
        return {};
    }

    auto const [cp, n] = get_cp(p + 1);
    if (not is_identifier_start(cp)) {
        return {};
    }

    auto r = token{++p, token::tag};
    p += n;

    while (p[0] != '\0') {
        auto const [cp, n] = get_cp(p);
        if (not is_identifier_continue(cp)) {
            r.set_last(p);
            return r;
        }

        p += n;
    }

    r.set_last(p);
    return r;
}

}

