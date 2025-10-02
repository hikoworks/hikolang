
#include "node.hpp"

namespace hk::ast {

class import_module_declaration_node : public node {
public:
    using node::node;

    import_module_declaration_node(file_location first, file_location last, fqname name, fqname as) :
        node(first, last), name(name), as(as)
    {
    }

    fqname name;
    fqname as;

private:
};

using module_node_ptr = std::unique_ptr<import_module_declaration_node>;

} // namespace hk::ast
