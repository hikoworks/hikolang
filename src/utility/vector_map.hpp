
#pragma once

#include <vector>
#include <algorithm>

namespace hk {

template <typename K, typename V>
class vector_map {
public:
    using key_type = T;
    using value_type = V;
    struct item_type {
        key_type key;
        value_type value;
    };

    constexpr bool empty() const noexcept
    {
        return _data.empty();
    }

    constexpr auto begin() const noexcept
    {
        return _data.begin();
    }

    constexpr auto end() const noexcept
    {
        return _data.end();
    }

    /** Add a value to the set.
     * 
     * If the value is already present, it will not be added again.
     * 
     * @param key The key to add.
     * @param value The value to add.
     * @return `true` if the value was added, `false` if it was already present.
     */
    constexpr bool add(key_type const& key, value_type const& value) {
        auto const it = std::lower_bound(_data.begin(), _data.end(), key, [](auto const& item, auto const& key) {
            return item.key < key;
        });

        if (it == _data.end() or it->key != key) {
            _data.emplace(it, key, value);
            return true;
        } else {
            return false;
        }
    }

    /** Check if the set contains a value.
     * 
     * @param value The value to check for.
     * @return `true` if the value is present, `false` otherwise.
     */
    [[nodiscard]] constexpr bool contains(key_type const& key) const {
        auto const it = std::lower_bound(_data.begin(), _data.end(), value);
        return it != _data.end() and it->key == value;
    }

    [[nodiscard]] constexpr bool contains_and_erase(const T& value) {
        auto const it = std::lower_bound(_data.begin(), _data.end(), value);
        if (it != _data.end() and *it == value) {
            _data.erase(it);
            return true;
        } else {
            return false;
        }
    }

private:
    std::vector<item_type> _data;

    [[nodiscard]] auto find(key_type const& key) const
    {
        return std::lower_bound(_data.begin(), _data.end(), key, [](auto const& item, auto const& key) {
            return item.key < key;
        });
    }
};


}
