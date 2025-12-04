

#pragma once

#include "node.hpp"
#include "import_repository_declaration_node.hpp"
#include "import_module_declaration_node.hpp"
#include "import_library_declaration_node.hpp"
#include <vector>

namespace hk::ast {

class repository;

class top_node : public node {
public:
    std::vector<import_repository_declaration_node_ptr> remote_repositories;
    std::vector<import_module_declaration_node_ptr> module_imports;
    std::vector<import_library_declaration_node_ptr> library_imports;
    std::vector<node_ptr> body;

    top_node(char const* first) : node(first) {}

    [[nodiscard]] hk::source& source() const override
    {
        assert(_source != nullptr);
        return *_source;
    }

    [[nodiscard]] generator<node *> children() const override
    {
        for (auto &ptr : remote_repositories) {
            co_yield ptr.get();
        }
        for (auto &ptr : module_imports) {
            co_yield ptr.get();
        }
        for (auto &ptr : library_imports) {
            co_yield ptr.get();
        }
        for (auto &ptr : body) {
            co_yield ptr.get();
        }
    }

    void fixup_top(hk::source *source)
    {
        assert(source != nullptr); 
        _source = source;
        fixup(nullptr);
    }

private:
    hk::source *_source = nullptr;
};

} // namespace hk::ast
