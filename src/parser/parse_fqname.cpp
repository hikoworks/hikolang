
#include "parsers.hpp"
#include "utility/fqname.hpp"
#include "error/errors.hpp"

namespace hk {

[[nodiscard]] parse_result<fqname> parse_fqname(token_iterator& it, error_list& e, bool leading_dot)
{
    auto const first = it->first;

    auto r = fqname{};

    if (*it == token::identifier) {
        r = std::move(it->text);
        ++it;

    } else if (leading_dot and it[0] == "." and it[1] == token::identifier) {
        r = ".";
        r += it[1]->text;
        it += 2;

    } else {
        return tokens_did_not_match;
    }

    while (*it == ".") {
        ++it;

        if (*it == token::identifier) {
            r += it->text;
            ++it;

        } else {
            return e.add(first, it->last, error::invalid_fqname);
        }
    }

    return r;
}

} // namespace hk