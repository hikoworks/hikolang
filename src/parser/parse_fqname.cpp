
#include "parse_fqname.hpp"
#include "utility/fqname.hpp"

namespace hk {

[[nodiscard]] parse_result<fqname> parse_fqname(token_iterator& it, parse_context& ctx)
{
    auto const first = it->begin();

    auto r = fqname{};

    if (*it == token::identifier) {
        if (auto id = it->identifier_value()) {
            r = *id;
            ++it;
        } else {
            ctx.add(first, it->end(), hkc_error::insecure_identifier, "identifier `{}`: {}", it->string_view(), id.error());
        }

    } else {
        return tokens_did_not_match;
    }

    while (*it == ".") {
        ++it;

        if (*it == token::identifier) {
            if (auto id = it->identifier_value()) {
                r += *id;
                ++it;
            } else {
                ctx.add(first, it->end(), hkc_error::insecure_identifier, "identifier `{}`: {}", it->string_view(), id.error());
            }

        } else {
            return ctx.add(first, it->end(), hkc_error::invalid_fqname);
        }
    }

    return r;
}

} // namespace hk