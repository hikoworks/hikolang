
#include "parse_fqname.hpp"
#include "utility/fqname.hpp"

namespace hk {

[[nodiscard]] static parse_result<fqname> parse_fqname(token_iterator& it, parse_context& ctx, bool absolute)
{
    using namespace std::literals;

    // Lookahead is maximum 8, dots followed by identifier.
    constexpr auto max_num_prefix_dots = 7uz;

    auto const first = it->begin();

    auto r = std::string{};

    auto num_prefix_dots = 0uz;
    if (absolute) {
        r = '.';

    } else {
        for (; num_prefix_dots != max_num_prefix_dots; ++num_prefix_dots) {
            if (it[num_prefix_dots] != '.') {
                break;
            }
            r += '.';
        }
        if (num_prefix_dots == max_num_prefix_dots) {
            return tokens_did_not_match;
        }
    }

    if (it[num_prefix_dots] == token::identifier) {
        if (auto id = it->identifier_value()) {
            r += *id;
            it += num_prefix_dots + 1;
        } else {
            ctx.add((it + num_prefix_dots)->begin(), (it + num_prefix_dots)->end(), hkc_error::insecure_identifier, "identifier `{}`: {}", it->string_view(), id.error());
        }

    } else {
        return tokens_did_not_match;
    }

    while (*it == ".") {
        r += '.';
        ++it;

        if (*it == token::identifier) {
            if (auto id = it->identifier_value()) {
                r += *id;
                ++it;
            } else {
                ctx.add(it->begin(), it->end(), hkc_error::insecure_identifier, "identifier `{}`: {}", it->string_view(), id.error());
            }

        } else {
            return ctx.add(first, it->end(), hkc_error::invalid_fqname);
        }
    }

    return fqname{std::move(r)};
}

[[nodiscard]] parse_result<fqname> parse_absolute_fqname(token_iterator& it, parse_context &ctx)
{
    return parse_fqname(it, ctx, true);
}

[[nodiscard]] parse_result<fqname> parse_relative_fqname(token_iterator& it, parse_context &ctx)
{
    return parse_fqname(it, ctx, false);
}

} // namespace hk