
#pragma once

#include "top_node.hpp"
#include "library_declaration_node.hpp"

namespace hk::ast {

class library_node : public top_node {
public:
    library_node(char const* first, std::unique_ptr<library_declaration_node> declaration) : top_node(first), _declaration(std::move(declaration)) {}

    [[nodiscard]] top_declaration_node &declaration() const override
    {
        return *_declaration;
    }

    [[nodiscard]] generator<node *> children() const override
    {
        co_yield _declaration.get();
    }

private:
    std::unique_ptr<library_declaration_node> _declaration;

};

using library_node_ptr = std::unique_ptr<library_node>;

} // namespace hk::ast
