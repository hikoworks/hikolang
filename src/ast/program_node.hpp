
#pragma once

#include "top_node.hpp"
#include "program_declaration_node.hpp"

namespace hk::ast {

class program_node : public top_node {
public:
    program_declaration_node_ptr declaration;

    program_node(char const* first) : top_node(first) {}
};

using program_node_ptr = std::unique_ptr<program_node>;

} // namespace hk::ast
