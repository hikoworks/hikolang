

#include "token_parsers.hpp"


namespace hl {

[[nodiscard]] std::optional<token> parse_positional_argument(file_cursor& c)
{
    if (c[0] != '$' or not is_digit(c[1])) {
        return std::nullopt;
    }

    auto r = token{c.location(), token::positional_argument};
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
