
#pragma once

#include "build_guard_expression_node.hpp"

namespace hk::ast {

class build_guard_binary_operator_node : public build_guard_expression_node {
public:
    enum class kind_type { none, _and, _or, in, not_in, eq, ne, lt, gt, le, ge };

    std::unique_ptr<build_guard_expression_node> lhs = {};
    std::unique_ptr<build_guard_expression_node> rhs = {};
    kind_type kind = kind_type::none;

    build_guard_binary_operator_node(char const* first, char const* last, kind_type kind) :
        build_guard_expression_node(first, last), kind(kind)
    {
    }

    [[nodiscard]] size_t precedence() const
    {
        switch (kind) {
        case kind_type::none: std::unreachable();
        case kind_type::_and: return 14;
        case kind_type::_or: return 15;
        case kind_type::in: return 5;
        case kind_type::not_in: return 5;
        case kind_type::eq: return 10;
        case kind_type::ne: return 10;
        case kind_type::lt: return 9;
        case kind_type::gt: return 9;
        case kind_type::le: return 9;
        case kind_type::ge: return 9;
        }
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