
#pragma once

#include "build_guard_expression_node.hpp"

namespace hk::ast {

class build_guard_unary_operator_node : public build_guard_expression_node {
public:
    std::unique_ptr<build_guard_expression_node> rhs = {};
    build_guard_value::un_op kind;

    build_guard_unary_operator_node(char const* first, char const* last, build_guard_value::un_op kind) :
        build_guard_expression_node(first, last), kind(kind)
    {
    }

    [[nodiscard]] std::expected<build_guard_value, hkc_error> evaluate(build_guard_context const& ctx) const override
    {
        auto const rhs_ = rhs->evaluate(ctx);
        if (not rhs_) {
            return std::unexpected{rhs_.error()};
        }

        if (auto optional_result = unary_op(kind, *rhs_)) {
            return std::move(optional_result).value();
        } else {
            return add(hkc_error::invalid_operand_types, "rhs: {}", rhs_->repr());
        }
    }

    [[nodiscard]] size_t precedence() const
    {
        // clang-format off
        switch (kind) {
        using enum build_guard_value::un_op;
        case _not: return 3;
        }
        // clang-format on
        std::unreachable();
    }

    [[nodiscard]] bool left_to_right() const
    {
        return false;
    }

    [[nodiscard]] generator<node*> children() const override
    {
        for (auto x : build_guard_expression_node::children()) {
            co_yield x;
        }

        if (rhs) {
            co_yield rhs.get();
        }
    }
};

using build_guard_unary_operator_node_ptr = std::unique_ptr<build_guard_unary_operator_node>;

} // namespace hk::ast