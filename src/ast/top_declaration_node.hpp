#pragma once

#include "node.hpp"
#include "build_guard_expression_node.hpp"
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
        assert(build_guard != nullptr);
        if (auto r = build_guard->evaluate_expression(ctx)) {
            _build_guard_result = static_cast<bool>(*r);
            return {};
        } else {
            _build_guard_result = false;
            return std::unexpected{r.error()};
        }
    }

    [[nodiscard]] bool enabled() noexcept
    {
        return _build_guard_result;
    }

private:
    bool _build_guard_result = false;
};

}