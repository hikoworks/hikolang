
#pragma once

#include "node.hpp"
#include "module_declaration_node.hpp"
#include "import_declaration_node.hpp"
#include "error/error_list.hpp"
#include "error/error_location.hpp"
#include "utility/repository_url.hpp"
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

    /** List of errors found.
     */
    mutable error_list errors;

    /** The path to the module file.
     */
    std::vector<std::filesystem::path> upstream_paths;

    /** The state of the module node.
     * 
     * This is used to track the state of the module node during parsing.
     * 
     * - `idle`: The module node was not parsed.
     * - `prologue`: The prologue of the module has been parsed.
     * - `parsed`: The module was fully parsed.
     */
    state_type state = state_type::idle;
    
    module_declaration_node_ptr declaration;
    std::vector<import_declaration_node_ptr> imports;
    std::vector<node_ptr> body;

    [[nodiscard]] generator<std::pair<repository_url, error_location>> remote_repositories() const;

};

using module_node_ptr = std::unique_ptr<module_node>;

} // namespace hk
