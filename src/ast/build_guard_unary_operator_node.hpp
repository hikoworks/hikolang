
#pragma once

#include "build_guard_expression_node.hpp"

namespace hk::ast {

class build_guard_unary_operator_node : public build_guard_expression_node {
public:
    enum class op_type {
        _not,
    };

    std::unique_ptr<build_guard_expression_node> rhs = {};
    op_type op;

    build_guard_unary_operator_node(char const* first, char const* last, op_type op) :
        build_guard_expression_node(first, last), op(op)
    {
    }

    [[nodiscard]] std::expected<datum, hkc_error> evaluate_expression(datum_namespace const& ctx) const override
    {
        auto const rhs_ = rhs->evaluate_expression(ctx);
        if (not rhs_) {
            return std::unexpected{rhs_.error()};
        }

        try {
            switch (op) {
            case op_type::_not:
                return not *rhs_;
            }
            std::unreachable();

        } catch (std::invalid_argument const&) {
            return add(hkc_error::invalid_operand_types, "rhs: {}", rhs_->repr());
        }
    }

    [[nodiscard]] size_t precedence() const
    {
        // clang-format off
        switch (op) {
        case op_type::_not: return 3;
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