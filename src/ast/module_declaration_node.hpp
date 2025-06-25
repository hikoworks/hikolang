
#pragma once

#include "node.hpp"
#include "condition_expression_node.hpp"
#include "utility/fqname.hpp"
#include <string>

namespace hl::ast {

class module_declaration_node : public node {
public:
    fqname name;
    std::unique_ptr<condition_expression_node> condition;

};

}
