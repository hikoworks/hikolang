

#pragma once

#include "node.hpp"
#include "import_repository_declaration_node.hpp"
#include "import_module_declaration_node.hpp"
#include "import_library_declaration_node.hpp"
#include <vector>

namespace hk::ast {

class top_node : public node {
public:
    std::vector<import_repository_declaration_node_ptr> remote_repositories;
    std::vector<import_module_declaration_node_ptr> module_imports;
    std::vector<import_library_declaration_node_ptr> library_imports;
    std::vector<node_ptr> body;

    top_node(file_location first) : node(first) {}
};

}
