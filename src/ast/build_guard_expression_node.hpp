
#pragma once

#include "node.hpp"

namespace hk::ast {

class build_guard_expression_node : public node {
public:
    build_guard_expression_node(char const* first, char const* last=nullptr) : node(first, last) {}
};

using build_guard_expression_node_ptr = std::unique_ptr<build_guard_expression_node>;

} // namespace hk::ast