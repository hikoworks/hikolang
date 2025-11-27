
#pragma once

#include "node.hpp"

namespace hk::ast {

class expression_node : public node {
public:
};

using expression_node_ptr = std::unique_ptr<expression_node>;

} // namespace hk::ast