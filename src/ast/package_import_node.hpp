
#pragma once

#include "node.hpp"

namespace hk::ast {

class package_import_node : public node {
public:
    using node::node;

    enum class kind_type {
        git,
        directory,
        zip,
    };

    /** The kind of package import.
     * 
     * This indicates how the package is imported, e.g. from a git repository, a directory, or a zip file.
     */
    kind_type kind = kind_type::directory;

    /** The location of the package, e.g. a git repository URL, a directory path, or a zip file path.
     * 
     * This is the location where the package can be found.
     */
    std::string location = {};

    /** The branch of the package to import.
     * 
     * This is only used for git packages, and specifies the branch to use.
     */
    std::string branch = {};

};

using package_import_node_ptr = std::unique_ptr<package_import_node>;

}
