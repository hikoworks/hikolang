
#pragma once

#include "node.hpp"
#include "expression_node.hpp"
#include "utility/semantic_version.hpp"
#include <string>

namespace hk::ast {

class program_declaration_node : public node {
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

    /** The compilation condition.
     *
     * Must be true for the module to be compiled.
     */
    expression_node_ptr condition = nullptr;

    bool is_fallback = false;

    program_declaration_node(char const* first) : node(first) {}

    [[nodiscard]] generator<node *> children() const override
    {
        if (condition) {
            co_yield condition.get();
        }
    }

};

using program_declaration_node_ptr = std::unique_ptr<program_declaration_node>;

} // namespace hk::ast
