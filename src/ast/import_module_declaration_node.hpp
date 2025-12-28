
#pragma once

#include "node.hpp"
#include "build_guard_expression_node.hpp"
#include "utility/fqname.hpp"

namespace hk::ast {

class import_module_declaration_node : public node {
public:
    fqname name = {};
    fqname as = {};

    std::unique_ptr<build_guard_expression_node> build_guard;

    import_module_declaration_node(char const* first) : node(first) {}


    std::expected<void, hkc_error> evaluate_build_guard(datum_namespace const& ctx) override
    {
        if (auto r = build_guard->evaluate_expression(ctx)) {
            _build_guard_result = static_cast<bool>(*r);
            return {};
        } else {
            _build_guard_result = false;
            return std::unexpected{r.error()};
        }
    }

    [[nodiscard]] generator<node *> children() const override
    {
        for (auto ptr : node::children()) {
            co_yield ptr;
        }

        assert(build_guard != nullptr);
        co_yield build_guard.get();
    }

    /** Is this top node enabled.
     * 
     * Reasons for this top node to be valid:
     *  * build-guard result is true.
     */
    [[nodiscard]] virtual bool enabled() noexcept
    {
        return _build_guard_result;
    }

private:
    bool _build_guard_result = false;
};

using import_module_declaration_node_ptr = std::unique_ptr<import_module_declaration_node>;

} // namespace hk::ast
