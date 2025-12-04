
#pragma once

#include "top_node.hpp"
#include "module_declaration_node.hpp"

namespace hk::ast {

class module_node : public top_node {
public:
    module_declaration_node_ptr declaration;

    module_node(char const* first) : top_node(first) {}

    [[nodiscard]] generator<node *> children() const override
    {
        co_yield declaration.get();
    }

};

using module_node_ptr = std::unique_ptr<module_node>;

} // namespace hk::ast
