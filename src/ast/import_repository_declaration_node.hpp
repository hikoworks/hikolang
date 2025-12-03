
#pragma once

#include "node.hpp"
#include "utility/repository_url.hpp"

namespace hk::ast {

class import_repository_declaration_node : public node {
public:
    repository_url url;

    import_repository_declaration_node(node &parent, char const* first) : node(first) {}
};

using import_repository_declaration_node_ptr = std::unique_ptr<import_repository_declaration_node>;

} // namespace hk::ast
