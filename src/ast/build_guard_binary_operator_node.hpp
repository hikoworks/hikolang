
#pragma once

#include "build_guard_expression_node.hpp"

namespace hk::ast {

class build_guard_binary_operator_node : public build_guard_expression_node {
public:
    std::unique_ptr<build_guard_expression_node> lhs = {};
    std::unique_ptr<build_guard_expression_node> rhs = {};
    build_guard_value::bin_op kind;

    build_guard_binary_operator_node(char const* first, char const* last, build_guard_value::bin_op kind) :
        build_guard_expression_node(first, last), kind(kind)
    {
    }

    [[nodiscard]] std::expected<build_guard_value, hkc_error> evaluate(build_guard_context const& ctx) const override
    {
        auto const lhs_ = lhs->evaluate(ctx);
        auto const rhs_ = rhs->evaluate(ctx);
        if (not lhs_) {
            return std::unexpected{lhs_.error()};
        }
        if (not rhs_) {
            return std::unexpected{rhs_.error()};
        }

        if (auto optional_result = binary_op(kind, *lhs_, *rhs_)) {
            return std::move(optional_result).value();
        } else {
            return add(hkc_error::invalid_operand_types, "lhs: {}, rhs: {}", lhs_->repr(), rhs_->repr());
        }
    }

    [[nodiscard]] size_t precedence() const
    {
        // clang-format off
        switch (kind) {
        using enum build_guard_value::bin_op;
        case _and: return 14;
        case _or: return 15;
        case in: return 5;
        case not_in: return 5;
        case eq: return 10;
        case ne: return 10;
        case lt: return 9;
        case gt: return 9;
        case le: return 9;
        case ge: return 9;
        }
        // clang-format on
        std::unreachable();
    }

    [[nodiscard]] bool left_to_right() const
    {
        return true;
    }

    [[nodiscard]] generator<node*> children() const override
    {
        for (auto x : build_guard_expression_node::children()) {
            co_yield x;
        }

        if (lhs) {
            co_yield lhs.get();
        }
        if (rhs) {
            co_yield rhs.get();
        }
    }
};

using build_guard_binary_operator_node_ptr = std::unique_ptr<build_guard_binary_operator_node>;

} // namespace hk::ast