
#include "parse_build_guard.hpp"
#include "parse_build_guard_expression.hpp"
#include "parse_build_guard_primary.hpp"

namespace hk {


[[nodiscard]] parse_result_ptr<ast::build_guard_expression_node> parse_build_guard(token_iterator& it, parse_context &ctx)
{
    auto const first = it->begin();

    if (*it != "if") {
        return tokens_did_not_match;
    }
    ++it;

    if (auto node = parse_build_guard_primary(it, ctx)) {

        if (auto top_node = parse_build_guard_expression(it, ctx, std::move(node).value(), 0)) {

        } else if (to_bool(node.error())) {
            return std::unexpected(node.error());
        } else {
            std::unreachable();
        }

    } else if (to_bool(node.error())) {
        return std::unexpected(node.error());
    } else {
        return ctx.add(it->begin(), hkc_error::missing_expression);
    }
}


}
