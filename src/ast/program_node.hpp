
#pragma once

#include "top_node.hpp"
#include "program_declaration_node.hpp"

namespace hk::ast {

class program_node : public top_node {
public:
    program_declaration_node_ptr declaration;

    program_node(char const* first) : top_node(first) {}

    [[nodiscard]] generator<node *> children() const override
    {
        co_yield declaration.get();
    }
};

using program_node_ptr = std::unique_ptr<program_node>;

} // namespace hk::ast
