
#pragma once

#include "node.hpp"
#include "expression_node.hpp"
#include "utility/fqname.hpp"
#include "utility/semantic_version.hpp"
#include "utility/enum_variant.hpp"
#include <string>
#include <filesystem>

namespace hk::ast {

class module_declaration_node : public node {
public:
    /** The name of the module.
     *
     * This is the fully qualified name of the module, e.g. `com.example.my_module`.
     */
    fqname name = {};

    /** The version of the module.
     *
     * This is the semantic version of the module, e.g. `1.0.0`.
     *
     * @note Valid when the module is a package.
     */
    semantic_version version = {};

    /** The compilation condition.
     *
     * Must be true for the module to be compiled.
     */
    expression_node_ptr condition = nullptr;

    /** Indicates if the module is a fallback module.
     *
     * A fallback module is a module that is compiled when no other module of
     * the same name will be compiled.
     */
    bool is_fallback = false;

    module_declaration_node(char const* first) : node(first) {}

    /** The result of the compilation condition.
     */
    [[nodiscard]] bool condition_result() const
    {
        return false;
    };
};

using module_declaration_node_ptr = std::unique_ptr<module_declaration_node>;

} // namespace hk::ast
