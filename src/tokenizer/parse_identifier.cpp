

#include "token_parsers.hpp"
#include "char_category.hpp"

namespace hk {

[[nodiscard]] token parse_identifier(char const*& p)
{
    auto [cp, n] = get_cp(p);
    if (not is_identifier_start(cp)) {
        return {};
    }

    auto r = token{p, token::identifier};
    p += n;

    while (p[0] != '\0') {
        auto [cp, n] = get_cp(p);
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

