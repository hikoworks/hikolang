
#include "parse_build_guard_expression.hpp"
#include "parse_build_guard_binary_operator.hpp"
#include "parse_build_guard_primary.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::build_guard_expression_node> parse_build_guard_expression(
    token_iterator& it,
    parse_context& ctx,
    std::unique_ptr<ast::build_guard_expression_node> lhs,
    size_t min_precedence)
{
    auto const first = it->begin();

    auto lookahead_it = it;
    auto lookahead = parse_build_guard_binary_operator(lookahead_it, ctx);
    while (lookahead and (*lookahead)->precedence() >= min_precedence) {
        auto op = std::move(lookahead).value();
        it = lookahead_it;

        auto rhs = std::unique_ptr<ast::build_guard_expression_node>{};
        if (auto optional_rhs = parse_build_guard_primary(it, ctx)) {
            rhs = std::move(optional_rhs).value();
        } else if (to_bool(optional_rhs.error())) {
            return std::unexpected{optional_rhs.error()};
        } else {
            return ctx.add(first, it->begin(), hkc_error::missing_rhs_of_binary_operator);
        }

        lookahead_it = it;
        lookahead = parse_build_guard_binary_operator(lookahead_it, ctx);
        while (lookahead and
               ((*lookahead)->precedence() > op->precedence() or
                ((*lookahead)->left_to_right() and (*lookahead)->precedence() == op->precedence()))) {
            auto const new_precedence = [&] {
                if ((*lookahead)->precedence() > op->precedence()) {
                    return op->precedence() + 1;
                } else {
                    return op->precedence();
                }
            }();

            if (auto optional_rhs = parse_build_guard_expression(it, ctx, std::move(rhs), new_precedence)) {
                rhs = std::move(optional_rhs).value();
            } else if (to_bool(optional_rhs.error())) {
                return std::unexpected{optional_rhs.error()};
            } else {
                std::unreachable();
            }

            lookahead_it = it;
            lookahead = parse_build_guard_binary_operator(lookahead_it, ctx);
        }

        op->lhs = std::move(lhs);
        op->rhs = std::move(rhs);
        lhs = std::move(op);
    }

    if (to_bool(lookahead.error())) {
        return std::unexpected{lookahead.error()};
    }

    return lhs;
}

} // namespace hk
