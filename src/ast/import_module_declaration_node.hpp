
#include "import_declaration_node.hpp"

namespace hk::ast {

class import_module_declaration_node : public import_declaration_node {
public:
    using import_declaration_node::import_declaration_node;

    import_module_declaration_node(file_location first, file_location last, fqname name, fqname as) :
        import_declaration_node(first, last), name(name), as(as)
    {
    }

    fqname name;
    fqname as;

private:
};

} // namespace hk::ast
