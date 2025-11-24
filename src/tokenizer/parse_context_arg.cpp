

#include "token_parsers.hpp"
#include "char_category.hpp"
#include <iterator>

namespace hk {

[[nodiscard]] std::optional<token> parse_context_arg(char const*& p)
{
    auto const location = p;

    if (p[0] != '$') {
        return std::nullopt;
    }

    auto t = token{location, token::tag};
    if (auto const [cp, n] = get_cp(p + 1); n != 0 and is_identifier_start(cp)) {
        p += n + 1;

    } else {
        return std::nullopt;
    }

    while (true) {
        auto const [cp, n] = get_cp(p);
        if (not is_identifier_continue(cp)) {
            // End of identifier, including end of file.
            t.append(std::string_view{location + 1, std::distance(location + 1, p)});
            t.last = p;
            return t.normalize_and_security_check();
        }

        p += n;
    }
}

}

