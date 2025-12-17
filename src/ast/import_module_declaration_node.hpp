
#pragma once

#include "node.hpp"
#include "build_guard_expression_node.hpp"
#include "utility/fqname.hpp"

namespace hk::ast {

class import_module_declaration_node : public node {
public:
    fqname name = {};
    fqname as = {};

    std::unique_ptr<build_guard_expression_node> build_guard;

    import_module_declaration_node(char const* first) : node(first) {}

private:
};

using import_module_declaration_node_ptr = std::unique_ptr<import_module_declaration_node>;

} // namespace hk::ast
