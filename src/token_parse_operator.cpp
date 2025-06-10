
#include "token_parsers.hpp"
#include "utf8.hpp"
#include <format>
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <string>
#include <string_view>
#include <expected>
#include <utility>

namespace hl {

[[nodiscard]] std::optional<token> parse_operator(file_cursor& c)
{
    if (not is_pattern_syntax(c[0])) {
        // Not an operator, return empty.
        return std::nullopt;
    }

    auto r = token{c.location(), token::_operator};
    r.append(c[0]);
    ++c;

    while (true) {
        if (not is_pattern_syntax(c[0])) {
            // End of operator, including end of file.
            r.last = c.location();
            return r;
        }

        r.append(c[0]);
        ++c;
    }
}

} // namespace hl
