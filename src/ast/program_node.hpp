
#pragma once

#include "top_node.hpp"
#include "program_declaration_node.hpp"

namespace hk::ast {

class program_node : public top_node {
public:
    program_node(char const* first, std::unique_ptr<program_declaration_node> declaration) : top_node(first), _declaration(std::move(declaration)) {}

    [[nodiscard]] top_declaration_node &declaration() const override
    {
        return *_declaration;
    }

    [[nodiscard]] generator<node *> children() const override
    {
        co_yield _declaration.get();
    }

    [[nodiscard]] bool enabled() noexcept override
    {
        assert(_declaration != nullptr);
        return _declaration->enabled();
    }

private:
    std::unique_ptr<program_declaration_node> _declaration;
};

using program_node_ptr = std::unique_ptr<program_node>;

} // namespace hk::ast
