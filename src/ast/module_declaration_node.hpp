
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
    using node::node;

    enum class kind_type {
        module,
        program,
        library
    };

    /** The type of module declaration.
     * 
     * This indicates the kind of module declaration, e.g. a module, package, application, or library.
     */
    kind_type kind = kind_type::module;

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

    /** The stem of the filename of a library or program.
     *
     * This is the filename where the module will be compiled to.
     * 
     * @note Valid when the module is an application or library.
     */
    std::string filename_stem = {};

    /** The compilation condition.
     * 
     * Must be true for the module to be compiled.
     */
    expression_node_ptr condition = nullptr;

    /** The result of the compilation condition.
     */
    [[nodiscard]] bool condition_result() const {
        return false;
    };

    /** Indicates if the module is a fallback module.
     * 
     * A fallback module is a module that is compiled when no other module of
     * the same name will be compiled.
     */
    bool is_fallback = false;
};

using module_declaration_node_ptr = std::unique_ptr<module_declaration_node>;

}
