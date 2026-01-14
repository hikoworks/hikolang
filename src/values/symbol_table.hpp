
#pragma once

#include "symbol.hpp"
#include "utility/fqname.hpp"
#include <memory>
#include <vector>
#include <compare>

namespace hk {

class symbol_table {
public:
    struct value_type {
        fqname name;
        std::shared_ptr<hk::symbol> symbol;

        [[nodiscard]] friend bool operator==(value_type const& lhs, value_type const& rhs)
        {
            return lhs.name == rhs.name;
        }

        [[nodiscard]] friend std::strong_ordering operator<=>(value_type const& lhs, value_type const& rhs)
        {
            return lhs.name <=> rhs.name;
        }
    };

    using container_type = std::vector<value_type>;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;

    constexpr symbol_table() = default;
    symbol_table(symbol_table const&) = default;
    symbol_table(symbol_table&&) = default;
    symbol_table& operator=(symbol_table const&) = default;
    symbol_table& operator=(symbol_table&&) = default;

    /** Create a symbol table by merging them together.
     */
    symbol_table(std::span<symbol_table const*> tables);

    [[nodiscard]] constexpr bool empty() const
    {
        return _table.empty();
    }

    [[nodiscard]] constexpr size_t size() const
    {
        return _table.size();
    }

    [[nodiscard]] constexpr iterator begin()
    {
        return _table.begin();
    }

    [[nodiscard]] constexpr const_iterator begin() const
    {
        return _table.begin();
    }

    [[nodiscard]] constexpr iterator end()
    {
        return _table.end();
    }

    [[nodiscard]] constexpr const_iterator end() const
    {
        return _table.end();
    }

    /** Add a symbol to the symbol table.
     * 
     * @param name The name of the symbol to insert.
     * @param symbol The symbol
     * @return The duplicate symbol.
     * @retval nullptr Successfully inserted.
     */
    hk::symbol* add(fqname name, std::shared_ptr<hk::symbol> symbol);

    /** Get a symbol from the symbol table.
     * 
     * @param name The name of the symbol to get.
     * @return The symbol that was found.
     * @retval nullptr The symbol was not found.
     */
    [[nodiscard]] hk::symbol* get(fqname const& name) const;

private:
    container_type _table;
};

} // namespace hk
