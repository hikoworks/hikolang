
#pragma once

#include "node.hpp"
#include "module_declaration_node.hpp"
#include "import_declaration_node.hpp"
#include <memory>
#include <vector>

namespace hl::ast {

class module_node : public node {
public:
    using node::node;
    
    std::unique_ptr<module_declaration_node> declaration;
    std::vector<std::unique_ptr<import_declaration_node>> imports;
    std::vector<std::unique_ptr<node>> body;

};

} // namespace hl
