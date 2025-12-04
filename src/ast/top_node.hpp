

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

    [[nodiscard]] hk::ast::repository& repository() const override
    {
        assert(_repository != nullptr);
        return *_repository;
    }

    top_node& set_repository(hk::ast::repository &repo) noexcept
    {
        _repository = &repo;
        return *this;
    }

private:
    hk::ast::repository *_repository = nullptr;
};

} // namespace hk::ast
