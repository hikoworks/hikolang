
#pragma once

#include "top_node.hpp"
#include "library_declaration_node.hpp"

namespace hk::ast {

class library_node : public top_node {
public:
    library_declaration_node_ptr declaration;

    library_node(char const* first) : top_node(first) {}

    [[nodiscard]] generator<node *> children() const override
    {
        co_yield declaration.get();
    }

};

using library_node_ptr = std::unique_ptr<library_node>;

} // namespace hk::ast
