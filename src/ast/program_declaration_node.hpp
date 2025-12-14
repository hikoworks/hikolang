
#pragma once

#include "top_declaration_node.hpp"
#include "build_guard_expression_node.hpp"
#include "utility/semantic_version.hpp"
#include <string>

namespace hk::ast {

class program_declaration_node : public top_declaration_node {
public:
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


    program_declaration_node(char const* first) : top_declaration_node(first) {}


};

using program_declaration_node_ptr = std::unique_ptr<program_declaration_node>;

} // namespace hk::ast
