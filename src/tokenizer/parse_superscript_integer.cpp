

#include "token_parsers.hpp"
#include "char_category.hpp"

namespace hk {

[[nodiscard]] token parse_superscript_integer(char const*& p)
{
    auto const [cp0, n0] = get_cp(p);
    auto const [cp1, n1] = get_cp(p + n0);
    auto const is_plusmin = cp0 == U'⁺' or cp0 == U'⁻';
    auto const is_number = is_superscript_digit(cp0) or (is_plusmin and is_superscript_digit(cp1));
    if (not is_number) {
        return {};
    }

    auto r = token{p, token::superscript_integer_literal};

    p += n0;
    if (cp0 == U'⁺' or cp0 == U'⁻') {
        p += n1;
    }

    while (p[0] != '\0') {
        auto const [cp, n] = get_cp(p);
        if (not is_superscript_digit(cp) and cp != U'\'') {
            r.set_last(p);
            return r;
        }

        p += n;
    }

    r.set_last(p);
    return r;
}

}
