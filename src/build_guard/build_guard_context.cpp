
#include "build_guard_context.hpp"

namespace hk {

[[nodiscard]] build_guard_value const* build_guard_context::get(fqname const& name) const
{
    auto const it = std::lower_bound(_items.begin(), _items.end(), name, [](auto const& item, auto const& x) {
        return item.name < x;
    });
    if (it == _items.end() or it->name != name) {
        return nullptr;
    }

    return std::addressof(it->value);
}

build_guard_value& build_guard_context::set(fqname name, build_guard_value value)
{
    auto it = std::lower_bound(_items.begin(), _items.end(), name, [](auto const& item, auto const& x) {
        return item.name < x;
    });
    if (it == _items.end() or it->name != name) {
        it = _items.emplace(it, std::move(name), std::move(value));
    }
    return it->value;
}

}
