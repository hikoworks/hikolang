
#pragma once

#include "top_declaration_node.hpp"
#include "utility/semantic_version.hpp"
#include <string>

namespace hk::ast {

class library_declaration_node : public top_declaration_node {
public:
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

    library_declaration_node(char const* first) : top_declaration_node(first) {}
};

using library_declaration_node_ptr = std::unique_ptr<library_declaration_node>;

} // namespace hk::ast
