
#include "datum_namespace.hpp"

namespace hk {

[[nodiscard]] datum const* datum_namespace::get(fqname const& name) const
{
    assert(name.is_absolute());

    auto const it = std::lower_bound(_items.begin(), _items.end(), name, [](auto const& item, auto const& x) {
        return item.name < x;
    });
    if (it == _items.end() or it->name != name) {
        return nullptr;
    }

    return std::addressof(it->value);
}

datum& datum_namespace::set(fqname name, datum value)
{
    assert(name.is_absolute());

    auto it = std::lower_bound(_items.begin(), _items.end(), name, [](auto const& item, auto const& x) {
        return item.name < x;
    });
    if (it == _items.end() or it->name != name) {
        it = _items.emplace(it, std::move(name), std::move(value));
    }
    return it->value;
}

void datum_namespace::remove(fqname const& name)
{
    assert(name.is_absolute());

    auto it = std::lower_bound(_items.begin(), _items.end(), name, [](auto const& item, auto const& x) {
        return item.name < x;
    });
    if (it == _items.end() or it->name != name) {
        return;
    }
    _items.erase(it);
}

}
