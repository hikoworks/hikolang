
#include "import_declaration_node.hpp"

namespace hk::ast {

class import_library_declaration_node : public import_declaration_node {
public:
    using import_declaration_node::import_declaration_node;

    import_library_declaration_node(file_location first, file_location last, std::filesystem::path path) :
        import_declaration_node(first, last), path(path)
    {
    }

    std::filesystem::path path;
};

} // namespace hk::ast
