
#include "parse_build_guard_primary.hpp"
#include "parse_build_guard_expression.hpp"
#include "parse_fqname.hpp"
#include "ast/build_guard_variable_node.hpp"
#include "ast/build_guard_literal_node.hpp"
#include "ast/build_guard_unary_operator_node.hpp"

namespace hk {


[[nodiscard]] parse_result_ptr<ast::build_guard_expression_node> parse_build_guard_primary(token_iterator& it, parse_context &ctx)
{
    auto const first = it->begin();

    if (auto optional_name = parse_absolute_fqname(it, ctx)) {
        return std::make_unique<ast::build_guard_variable_node>(first, it->begin(), std::move(optional_name).value());
    } else if (to_bool(optional_name.error())) {
        return std::unexpected{optional_name.error()};
    }

    if (*it == token::integer_literal) {
        ++it;
        return std::make_unique<ast::build_guard_literal_node>(first, it->begin(), it->integer_value());
    }

    if (*it == token::version_literal) {
        ++it;
        return std::make_unique<ast::build_guard_literal_node>(first, it->begin(), it->version_value());
    }

    if (*it == token::string_literal) {
        ++it;
        return std::make_unique<ast::build_guard_literal_node>(first, it->begin(), it->raw_string_value());
    }

    if (*it == "not") {
        ++it;

        if (auto optional_primary = parse_build_guard_primary(it, ctx)) {
            if (auto optional_expr = parse_build_guard_expression(it, ctx, std::move(optional_primary).value(), 0)) {
                auto op = std::make_unique<ast::build_guard_unary_operator_node>(first, it->begin(), build_guard_value::un_op::_not);
                op->rhs = std::move(optional_expr).value();
                return op;

            } else if (to_bool(optional_expr.error())) {
                return std::unexpected{optional_expr.error()};
            } else {
                std::unreachable();
            }

        } else if (to_bool(optional_primary.error())) {
            return std::unexpected{optional_primary.error()};
        } else {
            return ctx.add(first, it->begin(), hkc_error::missing_expression);
        }
    }

    if (*it == '(') {
        ++it;

        if (auto optional_primary = parse_build_guard_primary(it, ctx)) {
            if (auto optional_expr = parse_build_guard_expression(it, ctx, std::move(optional_primary).value(), 0)) {
                if (*it != ')') {
                    return ctx.add(first, it->begin(), hkc_error::missing_closing_parenthesis);
                }
                ++it;

                return std::move(optional_expr).value();

            } else if (to_bool(optional_expr.error())) {
                return std::unexpected{optional_expr.error()};
            } else {
                std::unreachable();
            }

        } else if (to_bool(optional_primary.error())) {
            return std::unexpected{optional_primary.error()};
        } else {
            return ctx.add(first, it->begin(), hkc_error::missing_expression);
        }
    }

    return tokens_did_not_match;
}


}
