
#include "node.hpp"

namespace hk::ast {

class import_library_declaration_node : public node {
public:
    using node::node;

    import_library_declaration_node(file_location first, file_location last, std::filesystem::path path) :
        node(first, last), path(path)
    {
    }

    std::filesystem::path path;
};

using module_node_ptr = std::unique_ptr<import_module_declaration_node>;

} // namespace hk::ast
