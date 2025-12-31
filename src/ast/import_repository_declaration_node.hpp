
#pragma once

#include "node.hpp"
#include "build_guard_expression_node.hpp"
#include "utility/logic.hpp"
#include "utility/repository_url.hpp"

namespace hk::ast {

class import_repository_declaration_node : public node {
public:
    repository_url url;

    std::unique_ptr<build_guard_expression_node> build_guard;

    import_repository_declaration_node(char const* first) : node(first) {}

    std::expected<void, hkc_error> evaluate_build_guard(datum_namespace const& ctx) override
    {
        if (build_guard == nullptr) {
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

    [[nodiscard]] generator<node *> children() const override
    {
        for (auto ptr : node::children()) {
            co_yield ptr;
        }

        if (build_guard) {
            co_yield build_guard.get();
        }
    }

    /** Is this top node enabled.
     * 
     * Reasons for this top node to be valid:
     *  * build-guard result is true.
     */
    [[nodiscard]] logic enabled() noexcept
    {
        return _build_guard_result;
    }

private:
    logic _build_guard_result = logic::F;
};

using import_repository_declaration_node_ptr = std::unique_ptr<import_repository_declaration_node>;

} // namespace hk::ast
