

#include "token_parsers.hpp"
#include "char_category.hpp"

namespace hk {

[[nodiscard]] std::optional<token> parse_position_arg(file_cursor& c)
{
    if (c[0] != '$' or not is_digit(c[1])) {
        return std::nullopt;
    }

    auto r = token{c.location(), token::position_arg};
    r.append(c[1]);
    c += 2;

    while (true) {
        if (not is_digit(c[0])) {
            // End of numbered argument, including end of file.
            r.last = c.location();
            return r;
        }

        r.append(c[0]);
        ++c;
    }
}

}
