
#include "parsers.hpp"
#include "utility/fqname.hpp"
#include "error/errors.hpp"

namespace hk {

[[nodiscard]] parse_result<fqname> parse_fqname(token_iterator& it, error_list& e)
{
    auto const first = it->first;

    if (*it != token::identifier) {
        return tokens_did_not_match;
    }

    auto r = fqname{std::move(it->text)};
    ++it;

    while (true) {
        if (*it != ".") {
            return r;
        }

        ++it;
        if (*it != token::identifier) {
            return e.add(first, it->last, error::missing_fqname_identifier_after_dot);
        }

        r += it->text;
        ++it;
    }
}

} // namespace hk