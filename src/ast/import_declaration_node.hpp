
#pragma once

#include "node.hpp"
#include "utility/fqname.hpp"


namespace hl::ast {

class import_declaration_node : public node {
public:
    using node::node;

    /** The fully qualified name of the imported module.
     */
    fqname name;

};

}
