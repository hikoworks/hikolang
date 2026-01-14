
#include "symbol_table.hpp"
#include "utility/algorithm.hpp"

namespace hk {

symbol_table::symbol_table(std::span<symbol_table const*> tables)
{
    auto tables_ = std::vector<container_type const*>{};
    tables_.reserve(tables.size());
    for (auto const& table : tables) {
        tables_.push_back(std::addressof(table->_table));
    }

    multi_merge(tables_, _table);
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