
#pragma once

#include "node.hpp"
#include "utility/fqname.hpp"


namespace hk::ast {

class import_declaration_node : public node {
public:
    using node::node;

    enum class kind_type {
        /** Import a module.
         */
        mod,

        /** Import a git repository.
         */
        git,

        /** Import a zip-file repository.
         */
        zip,

        /** Import a FFI/system library.
         */
        lib
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

    /** Path to the repository or library.
     */
    std::string path = {};

    /** Git brach/tag/sha to checkout. 
     */
    std::string branch = {};


    /** Create the dependency directory-name.
     * 
     * This is based around the following parts:
     *  - The filename (excluding extension)
     *  - A dash `-`
     *  - A hash of the full path and branch.
     */
    [[nodiscard]] std::string directory_name() const;

private:
    [[nodiscard]] std::string path_hash() const;
    [[nodiscard]] std::string path_stem() const;

};

using import_declaration_node_ptr = std::unique_ptr<import_declaration_node>;

}
