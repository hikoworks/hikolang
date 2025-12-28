
#pragma once

#include "build_guard_expression_node.hpp"

namespace hk::ast {

class build_guard_binary_operator_node : public build_guard_expression_node {
public:
    enum class op_type {
        _and,
        _or,
        in,
        not_in,
        eq,
        ne,
        lt,
        gt,
        le,
        ge
    };

    std::unique_ptr<build_guard_expression_node> lhs = {};
    std::unique_ptr<build_guard_expression_node> rhs = {};
    op_type op;

    build_guard_binary_operator_node(char const* first, char const* last, op_type op) :
        build_guard_expression_node(first, last), op(op)
    {
    }

    [[nodiscard]] std::expected<datum, hkc_error> evaluate_expression(datum_namespace const& ctx) const override
    {
        assert(lhs != nullptr);
        auto const lhs_ = lhs->evaluate_expression(ctx);
        if (not lhs_) {
            return std::unexpected{lhs_.error()};
        }

        assert(rhs != nullptr);
        auto const rhs_ = rhs->evaluate_expression(ctx);
        if (not rhs_) {
            return std::unexpected{rhs_.error()};
        }

        try {
            switch (op) {
            case op_type::_and: return *lhs_ and *rhs_;
            case op_type::_or: return *lhs_ or *rhs_;
            case op_type::in: return in(*lhs_, *rhs_);
            case op_type::not_in: return not in(*lhs_, *rhs_);
            case op_type::eq: return *lhs_ == *rhs_;
            case op_type::ne: return *lhs_ != *rhs_;
            case op_type::lt: return *lhs_ < *rhs_;
            case op_type::gt: return *lhs_ > *rhs_;
            case op_type::le: return *lhs_ <= *rhs_;
            case op_type::ge: return *lhs_ >= *rhs_;
            }
            std::unreachable();
        } catch (std::invalid_argument const&) {
            return add(hkc_error::invalid_operand_types, "lhs: {}, rhs: {}", lhs_->repr(), rhs_->repr());
        }
    }

    [[nodiscard]] size_t precedence() const
    {
        // clang-format off
        switch (op) {
        case op_type::_and: return 14;
        case op_type::_or: return 15;
        case op_type::in: return 5;
        case op_type::not_in: return 5;
        case op_type::eq: return 10;
        case op_type::ne: return 10;
        case op_type::lt: return 9;
        case op_type::gt: return 9;
        case op_type::le: return 9;
        case op_type::ge: return 9;
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