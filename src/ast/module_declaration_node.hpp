
#pragma once

#include "node.hpp"
#include "condition_expression_node.hpp"
#include "utility/fqname.hpp"
#include <string>

namespace hk::ast {

class module_declaration_node : public node {
public:
    using node::node;

    fqname name;
    std::unique_ptr<condition_expression_node> condition;
    bool is_fallback = false;
};

}
