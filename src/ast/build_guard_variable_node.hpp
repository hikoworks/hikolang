
#pragma once

#include "build_guard_expression_node.hpp"
#include "utility/fqname.hpp"

namespace hk::ast {

class build_guard_variable_node : public build_guard_expression_node {
public:
    fqname name;

    build_guard_variable_node(char const *first, char const *last, fqname name) : build_guard_expression_node(first, last), name(name) {}

};

}