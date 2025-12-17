
#pragma once

#include "build_guard_expression_node.hpp"
#include "utility/fqname.hpp"

namespace hk::ast {

class build_guard_variable_node : public build_guard_expression_node {
public:
    fqname name;

    build_guard_variable_node(char const *first, char const *last, fqname name) : build_guard_expression_node(first, last), name(name) {}

    [[nodiscard]] std::expected<datum, hkc_error> evaluate(datum_namespace const& ctx) const override
    {
        if (auto optional_value = ctx.get(name)) {
            return *optional_value;
        } else {
            return add(hkc_error::unknown_build_guard_constant, "{}", name);
        }
    }

};

}