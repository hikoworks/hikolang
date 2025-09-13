
#pragma once

#include "node.hpp"
#include "utility/fqname.hpp"
#include "utility/generator.hpp"


namespace hk::ast {

class import_declaration_node : public node {
public:
    using node::node;

};

using import_declaration_node_ptr = std::unique_ptr<import_declaration_node>;

}
