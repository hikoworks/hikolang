
#pragma once

#include "node.hpp"
#include "expression_node.hpp"
#include "utility/semantic_version.hpp"
#include <string>

namespace hk::ast {

class library_declaration_node : public node {
public:
    using node::node;

    /** The stem of the filename of the library.
     *
     * This is the filename where the module will be compiled to.
     */
    std::string filename_stem = {};

    /** The version of the library.
     *
     * Multiple versions of the same dynamic library can coexist, this version
     * number is used to set the library's version in a operating system
     * depended way.
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
