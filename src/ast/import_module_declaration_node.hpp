
#pragma once

#include "node.hpp"
#include "utility/fqname.hpp"

namespace hk::ast {

class import_module_declaration_node : public node {
public:
    import_module_declaration_node(char const* first) : node(first) {}

    fqname name = {};
    fqname as = {};

private:
};

using import_module_declaration_node_ptr = std::unique_ptr<import_module_declaration_node>;

} // namespace hk::ast
