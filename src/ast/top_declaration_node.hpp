#pragma once

#include "node.hpp"
#include "build_guard_expression_node.hpp"
#include "utility/logic.hpp"
#include <string>

namespace hk::ast {

class top_declaration_node : public node {
public:
    top_declaration_node(char const* first) : node(first) {}

    /** The build-guard.
     *
     * Must be true for the program to be compiled.
     */
    build_guard_expression_node_ptr build_guard = nullptr;


    [[nodiscard]] generator<node *> children() const override
    {
        for (auto x: node::children()) {
            co_yield x;
        }

        if (build_guard) {
            co_yield build_guard.get();
        }
    }

    std::expected<void, hkc_error> evaluate_build_guard(datum_namespace const& ctx) override
    {
        if (build_guard == nullptr) {
            // Fallback.
            _build_guard_result = logic::_;
            return {};

        } else if (auto r = build_guard->evaluate_expression(ctx)) {
            _build_guard_result = to_logic(static_cast<bool>(*r));
            return {};

        } else {
            _build_guard_result = logic::X;
            return std::unexpected{r.error()};
        }
    }

    [[nodiscard]] logic enabled() noexcept
    {
        return _build_guard_result;
    }

private:
    logic _build_guard_result = logic::F;
};

}