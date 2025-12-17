
#pragma once

#include "node.hpp"
#include "build_guard_expression_node.hpp"
#include "utility/repository_url.hpp"

namespace hk::ast {

class import_repository_declaration_node : public node {
public:
    repository_url url;

    std::unique_ptr<build_guard_expression_node> build_guard;

    import_repository_declaration_node(char const* first) : node(first) {}
};

using import_repository_declaration_node_ptr = std::unique_ptr<import_repository_declaration_node>;

} // namespace hk::ast
