
#include "node.hpp"
#include "repository/source.hpp"

namespace hk::ast {


std::unexpected<hkc_error> node::_add(hkc_error error, std::string message) const
{
    auto const [it, _] = source().errors().add(source().lines(), first, last, error, std::move(message));
    return std::unexpected{it->code()};
}

std::expected<void, hkc_error> node::evaluate_build_guard(datum_namespace const& ctx)
{
    auto last_error = hkc_error::none;

    for (auto child_ptr : children()) {
        assert(child_ptr != nullptr);
        if (auto r = child_ptr->evaluate_build_guard(ctx); not r) {
            last_error = r.error();
        }
    }

    if (last_error != hkc_error::none) {
        return std::unexpected{last_error};
    }

    return {};
}

}