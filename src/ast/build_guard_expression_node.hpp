
#pragma once

#include "node.hpp"
#include "utility/datum_namespace.hpp"

namespace hk::ast {

class build_guard_expression_node : public node {
public:
    build_guard_expression_node(char const* first, char const* last=nullptr) : node(first, last) {}

    [[nodiscard]] virtual std::expected<datum, hkc_error> evaluate_expression(datum_namespace const& ctx) const = 0;
};

using build_guard_expression_node_ptr = std::unique_ptr<build_guard_expression_node>;

} // namespace hk::ast