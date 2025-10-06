

#include "token_parsers.hpp"
#include "char_category.hpp"

namespace hk {

[[nodiscard]] std::optional<token> parse_context_arg(file_cursor& c)
{
    if (c[0] != '$' or not is_identifier_start(c[1])) {
        return std::nullopt;
    }

    auto r = token{c.location(), token::tag};
    ++c;
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

