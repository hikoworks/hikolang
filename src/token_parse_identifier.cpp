

#include "token_parsers.hpp"
#include "char_category.hpp"

namespace hl {

[[nodiscard]] std::optional<token> parse_identifier(file_cursor& c)
{
    if (not is_identifier_start(c[0]) and c[0] != '$') {
        return std::nullopt;
    }

    auto r = token{c.location(), token::identifier};
    r.append(c[0]);
    ++c;

    while (true) {
        if (not is_identifier_continue(c[0])) {
            // End of identifier, including end of file.
            r.last = c.location();
            return r.normalize_and_security_check();
        }

        r.append(c[0]);
        ++c;
    }
}

}

