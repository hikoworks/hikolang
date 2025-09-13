
#pragma once

#include "node.hpp"
#include "utility/fqname.hpp"
#include "utility/generator.hpp"
#include "utility/remote_repo_url.hpp"


namespace hk::ast {

class import_declaration_node : public node {
public:
    using node::node;

    enum class kind_type {
        /** Import a module.
         */
        mod,

        /** Import a FFI/system library.
         */
        lib,

        /** Import a repository.
         */
        repository
    };

    /** The kind of import
     */
    kind_type kind = kind_type::mod;

    /** The fully qualified name of the imported module.
     */
    fqname name = {};

    /** The name to import the module to.
     */
    std::string as = {};

    remote_repo_url url = {};

private:

};

using import_declaration_node_ptr = std::unique_ptr<import_declaration_node>;

}
