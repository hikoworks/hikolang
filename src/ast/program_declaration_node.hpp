
#pragma once

#include "node.hpp"
#include "expression_node.hpp"
#include "utility/semantic_version.hpp"
#include <string>

namespace hk::ast {

class program_declaration_node : public node {
public:
    using node::node;

    /** The stem of the filename of the program.
     *
     * This is the filename where the module will be compiled to.
     */
    std::string filename_stem = {};

    /** The optional version of the program.
     * 
     * This is used so that the program could display its version number with
     * for example a "--version" command line argument.
     */
    semantic_version version = {};

    /** The compilation condition.
     * 
     * Must be true for the module to be compiled.
     */
    expression_node_ptr condition = nullptr;

    bool is_fallback = false;
};

}
