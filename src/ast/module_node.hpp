
#pragma once

#include "node.hpp"
#include "module_declaration_node.hpp"
#include "import_declaration_node.hpp"
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

    
    module_declaration_node_ptr declaration;
    std::vector<import_declaration_node_ptr> imports;
    std::vector<node_ptr> body;

    [[nodiscard]] generator<std::pair<repository_url, error_location>> remote_repositories() const;

};

using module_node_ptr = std::unique_ptr<module_node>;

} // namespace hk
