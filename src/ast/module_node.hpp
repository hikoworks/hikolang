
#pragma once

#include "node.hpp"
#include "module_declaration_node.hpp"
#include "import_declaration_node.hpp"
#include <memory>
#include <vector>
#include <chrono>

namespace hk::ast {

class module_node : public node {
public:
    using node::node;

    enum class state_type {
        idle,
        prologue,
        parsed,
    };

    /** The path to the module file.
     */
    std::filesystem::path path;

    /** Date when the module was parsed.
     * This is updated when the module parsing is completed.
     */
    std::chrono::system_clock::time_point parsed_at = {};

    /** The state of the module node.
     * 
     * This is used to track the state of the module node during parsing.
     * 
     * - `idle`: The module node was not parsed.
     * - `prologue`: The prologue of the module has been parsed.
     * - `parsed`: The module was fully parsed.
     */
    state_type state = state_type::idle;
    
    std::unique_ptr<module_declaration_node> declaration;
    std::vector<std::unique_ptr<import_declaration_node>> imports;
    std::vector<std::unique_ptr<node>> body;

};

} // namespace hk
