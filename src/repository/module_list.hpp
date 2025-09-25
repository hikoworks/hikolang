
#pragma once

#include "module.hpp"
#include <vector>
#include <algorithm>
#include <filesystem>

namespace hk {


class module_list {
public:
    using value_type = module_t;
    using pointer = value_type *;
    using reference = value_type&;
    using const_reference = value_type const&;
    using container_type = std::vector<value_type>;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;

    [[nodiscard]] std::size_t size() const noexcept
    {
        return _modules.size();
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return _modules.empty();
    }

    [[nodiscard]] iterator begin() noexcept
    {
        return _modules.begin();
    }

    [[nodiscard]] const_iterator begin() const noexcept
    {
        return _modules.begin();
    }

    [[nodiscard]] iterator end() noexcept
    {
        return _modules.end();
    }

    [[nodiscard]] const_iterator end() const noexcept
    {
        return _modules.end();
    }

    /** Insert a module at path.
     *
     * Only if a module with the same path does not exist will a new module be
     * inserted.
     * 
     * @param path The path to be inserted.
     * @return The iterator pointing to the module, true if inserted, false if
     *         it already existed.
     */
    std::pair<iterator, bool> insert(std::filesystem::path const& path);

    iterator erase(iterator it)
    {
        return _modules.erase(it);
    }

    const_iterator erase(const_iterator it)
    {
        return _modules.erase(it);
    }

    bool erase(std::filesystem::path const& path);

    iterator find(std::filesystem::path const& path);

private:
    enum class state_type {
        unordered,
        ordered_by_path,
        compilation_order
    };

    state_type _state = state_type::ordered_by_path;

    container_type _modules = {};

    void sort_by_path();
    void sort_by_compilation();
};


}
