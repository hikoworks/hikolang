
#include "symbol_table.hpp"
#include "utility/algorithm.hpp"
#include <algorithm>

namespace hk {

symbol_table::symbol_table(std::span<symbol_table const*> tables) : _tables()
{
    auto const size = std::accumulate(tables.begin(), tables.end(), 0uz, [](auto const& item) {
        return item.size();
    });
    _tables.reserve(size)

    for (auto const& table : tables) {
        _table.append_range(*table);
    }

    std::sort(_tables.begin(), _tables.end());
}

hk::symbol *symbol_table::add(fqname name, std::shared_ptr<hk::symbol> symbol)
{
    auto it = std::lower_bound(_table.begin(), _table.end(), name, [](auto const& item, auto const& x) {
        return item.name < x;
    });

    if (it != _table.end() and it->name == name) {
        return *it->symbol;
    }

    _table.emplace(it, std::move(name), std::move(symbol));
    return nullptr;
}

[[nodiscard]] hk::symbol *symbol_table::get(fqname const& name) const
{
    auto it = std::lower_bound(_table.begin(), _table.end(), name, [](auto const& item, auto const& x) {
        return item.name < x;
    });

    if (it != _table.end() and it->name == name) {
        return *it->symbol;
    }

    return nullptr;
}


} // namespace hk