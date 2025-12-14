#pragma once

#include "node.hpp"
#include "build_guard_expression_node.hpp"
#include <string>

namespace hk::ast {

class top_declaration_node : public node {
public:
    top_declaration_node(char const* first) : node(first) {}

    /** The build-guard.
     *
     * Must be true for the program to be compiled.
     */
    build_guard_expression_node_ptr build_guard = nullptr;

    [[nodiscard]] generator<node *> children() const override
    {
        for (auto x: node::children()) {
            co_yield x;
        }

        if (build_guard) {
            co_yield build_guard.get();
        }
    }
};

}