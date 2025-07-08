
#pragma once

#include "node.hpp"
#include "expression_node.hpp"
#include "utility/fqname.hpp"
#include "utility/semantic_version.hpp"
#include "utility/enum_variant.hpp"
#include <string>

namespace hk::ast {

class module_declaration_node : public node {
public:
    using node::node;

    enum class variant_index {
        module,
        package,
        application,
        library
    };
    using variant_type = enum_variant<variant_index, std::monostate, semantic_version, std::string, std::string>;

    fqname name;

    /** Holds data specific to the type of module declaration.
     * 
     * Belowe a list of the possible values:
     *  - `module`: No additional data.
     *  - `package`: The version of the package.
     *  - `application`: The name of the executable.
     *  - `library`: The name of the library.
     */
    variant_type variant;

    std::unique_ptr<expression_node> condition;
    bool is_fallback = false;
};

}
