
#pragma once

#include "build_guard_expression_node.hpp"
#include "utility/datum.hpp"

namespace hk::ast {

class build_guard_literal_node : public build_guard_expression_node {
public:
    datum value;

    build_guard_literal_node(char const* first, char const* last, long long value) :
        build_guard_expression_node(first, last), value(value) {}
    build_guard_literal_node(char const* first, char const* last, std::string value) :
        build_guard_expression_node(first, last), value(std::move(value)) {}
    build_guard_literal_node(char const* first, char const* last, semantic_version value) :
        build_guard_expression_node(first, last), value(std::move(value)) {}

    [[nodiscard]] std::expected<datum, hkc_error> evaluate(datum_namespace const& ctx) const override
    {
        return value;
    }

};

}