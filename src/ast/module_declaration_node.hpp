
#pragma once

#include "top_declaration_node.hpp"
#include "utility/fqname.hpp"
#include "utility/semantic_version.hpp"
#include "utility/enum_variant.hpp"
#include <string>
#include <filesystem>

namespace hk::ast {

class module_declaration_node : public top_declaration_node {
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

    module_declaration_node(char const* first) : top_declaration_node(first) {}

    /** The result of the compilation condition.
     */
    [[nodiscard]] bool condition_result() const
    {
        return false;
    };


};

using module_declaration_node_ptr = std::unique_ptr<module_declaration_node>;

} // namespace hk::ast
