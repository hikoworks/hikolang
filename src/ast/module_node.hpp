
#pragma once

#include "node.hpp"
#include "import_repository_declaration_node.hpp"
#include "import_module_declaration_node.hpp"
#include "import_library_declaration_node.hpp"
#include "error/error_list.hpp"
#include "error/error_location.hpp"
#include "utility/repository_url.hpp"
#include <memory>
#include <vector>
#include <chrono>

namespace hk::ast {

class module_node : public node {
public:
    using node::node;

    
    node_ptr declaration;
    std::vector<import_repository_declaration_node_ptr> remote_repositories;
    std::vector<import_module_declaration_node_ptr> module_imports;
    std::vector<import_library_declaration_node_ptr> library_imports;
    std::vector<node_ptr> body;


};

using module_node_ptr = std::unique_ptr<module_node>;

} // namespace hk
