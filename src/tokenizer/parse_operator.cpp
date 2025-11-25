
#include "token_parsers.hpp"
#include "char_category.hpp"
#include <format>
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <string>
#include <string_view>
#include <expected>
#include <utility>

namespace hk {

[[nodiscard]] token parse_operator(char const*& p)
{
    auto const [cp, n] = get_cp(p);
    if (not is_pattern_syntax(cp)) {
        // Not an operator, return empty.
        return {};
    }

    auto r = token{p, token::_operator};
    p += n;

    while (p[0] != '\0') {
        auto const [cp, n] = get_cp(p);
        if (not is_pattern_syntax(cp)) {
            // End of operator, including end of file.
            r.set_last(p);
            return r;
        }

        p += n;
    }

    r.set_last(p);
    return r;
}

} // namespace hk
