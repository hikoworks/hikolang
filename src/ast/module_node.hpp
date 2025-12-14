
#pragma once

#include "top_node.hpp"
#include "module_declaration_node.hpp"

namespace hk::ast {

class module_node : public top_node {
public:
    module_node(char const* first, std::unique_ptr<module_declaration_node> declaration) : top_node(first), _declaration(std::move(declaration)) {}

    [[nodiscard]] top_declaration_node &declaration() const override
    {
        return *_declaration;
    }

    [[nodiscard]] generator<node *> children() const override
    {
        co_yield _declaration.get();
    }

private:
    std::unique_ptr<module_declaration_node> _declaration;

};

using module_node_ptr = std::unique_ptr<module_node>;

} // namespace hk::ast
