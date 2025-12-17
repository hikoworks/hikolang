
#pragma once

#include "node.hpp"
#include <filesystem>

namespace hk::ast {

class import_library_declaration_node : public node {
public:
    std::filesystem::path path;

    import_library_declaration_node(char const* first, std::filesystem::path path) :
        node(first), path(path)
    {
    }

};

using import_library_declaration_node_ptr = std::unique_ptr<import_library_declaration_node>;

} // namespace hk::ast
