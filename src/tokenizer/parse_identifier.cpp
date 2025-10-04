

#include "token_parsers.hpp"
#include "utility/char_category.hpp"

namespace hk {

[[nodiscard]] std::optional<token> parse_identifier(file_cursor& c)
{
    if (not (is_identifier_start(c[0]) or c[0] == U'°')) {
        return std::nullopt;
    }

    auto r = token{c.location(), token::identifier};
    r.append(c[0]);
    ++c;

    while (true) {
        if (not (is_identifier_continue(c[0]) or c[0] == U'°')) {
            // End of identifier, including end of file.
            r.last = c.location();
            return r.normalize_and_security_check();
        }

        r.append(c[0]);
        ++c;
    }
}

}

