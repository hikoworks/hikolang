
#pragma once

#include "node.hpp"
#include "build_guard/build_guard_context.hpp"

namespace hk::ast {

class build_guard_expression_node : public node {
public:
    build_guard_expression_node(char const* first, char const* last=nullptr) : node(first, last) {}

    [[nodiscard]] virtual std::expected<build_guard_value, hkc_error> evaluate(build_guard_context const& ctx) const = 0;
};

using build_guard_expression_node_ptr = std::unique_ptr<build_guard_expression_node>;

} // namespace hk::ast