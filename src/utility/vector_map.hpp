
#pragma once

#include <vector>
#include <algorithm>

namespace hk {

/** A map based on a sorted std::vector
 * 
 * The vector maintains a list of sorted items for quick searching.
 * 
 * New items are appended to the vector, a duplicate key check is
 * performed on both the sorted and unsorted items in the vector.
 * 
 * @tparam K Type of the key
 * @tparam V Type of the value (maybe void)
 */
template <typename K, typename V>
class vector_map {
public:
    using key_type = K;
    using value_type = std::conditional_t<std::is_void_v<V>, std::monostate, V>;
    struct item_type {
        key_type key;
        [[no_unique_address]] value_type value;

        [[nodiscard]] constexpr friend bool operator<(item_type const& lhs, item_type const& rhs)
        {
            return lhs.key < rhs.key;
        }
    };

    using container_type = std::vector<item_type>;
    using iterator = container_type::iterator;
    using const_iterator = container_type::const_iterator;
    using size_type = container_type::size_type;

    /** The maximum number of unsorted entries at end of vector.
     * 
     * When the vector has 256 or more entries, than the number of
     * comparisons of std::lower_bound() is about 8. std::find() does
     * a faster linear search, so we choose 16.
     * 
     * 24 would also be reasonable, but the number of entries may also
     * be lower than 256.
     */
    constexpr static size_type max_unsorted_size = 16;

    constexpr ~vector_map() = default;
    constexpr vector_map(vector_map const&) = default;
    constexpr vector_map(vector_map&&) = default;
    constexpr vector_map& operator=(vector_map const&) = default;
    constexpr vector_map& operator=(vector_map&&) = default;
    constexpr vector_map() = default;

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return _data.empty();
    }

    [[nodiscard]] constexpr size_type size() const noexcept
    {
        return _data.size();
    }

    [[nodiscard]] constexpr size_type capacity() const noexcept
    {
        return _data.capacity();
    }

    [[nodiscard]] constexpr const_iterator begin() const noexcept
    {
        return _data.begin();
    }

    [[nodiscard]] constexpr const_iterator end() const noexcept
    {
        return _data.end();
    }

    [[nodiscard]] constexpr iterator begin() noexcept
    {
        return _data.begin();
    }

    [[nodiscard]] constexpr iterator end() noexcept
    {
        return _data.end();
    }

    /** Reserve space in the map.
     * 
     * @note This function does nothing if @a n is less than the current capacity.
     * @param n The number of entries to reserve.
     */
    constexpr void reserve(size_type n)
    {
        return _data.reserve(n);
    }

    /** Consolidate the sorted and unsorted part of the map.
     * 
     * If the number of unsorted entries is less than @a n nothing
     * happens. When no arguments are passed the items will be sorted.
     * 
     * @param n Maximum allowed unsorted entries.
     */
    constexpr void consolidate(size_type n = 0)
    {
        if (_unsorted_size > n) {
            std::sort(_middle(), end());
            std::inplace_merge(begin(), _middle(), end());
            _unsorted_size = 0;
        }
    }

    constexpr void clear()
    {
        _unsorted_size = 0;
        return _data.clear();
    }

    [[nodiscard]] constexpr const_iterator lower_bound(key_type const& key) const
    {
        const_cast<vector_map*>(this)->consolidate();
        return _lower_bound(key);
    }


    /** Find the key in the map.
     * 
     * @note Calls `consolidate()`.
     * @param key The key to find.
     * @return Iterator to the item found, or end().
     */
    [[nodiscard]] constexpr const_iterator find(key_type const& key) const
    {
        if (auto const it = lower_bound(key); it != end() and it->key == key) {
            return it;
        } else {
            return end();
        }
    }

    /** Check if the set contains a key.
     * 
     * @note Calls `consolidate()`.
     * @param key The key to check for
     * @return `true` if the value is present, `false` otherwise.
     */
    [[nodiscard]] constexpr bool contains(key_type const& key) const {
        if (auto const it = lower_bound(key); it != end() and it->key == key) {
            return true;
        } else {
            return false;
        }
    }

    /** Add a value to the set.
     * 
     * If the value is already present, it will not be added again.
     * 
     * @param key The key to add.
     * @param Args The arguments to construct value.
     * @return {iterator to element with key, inserted}
     */
    template<typename Key, typename... Args>
    constexpr std::pair<iterator, bool> try_emplace(Key&& key, Args&&... args)
    {    
        const_cast<vector_map*>(this)->consolidate(max_unsorted_size);

        if (auto const it = _find(key); it != end() and it->key == key) {
            return {it, false};
        }

        auto const it = _data.emplace(_data.end(), std::forward<Key>(key), value_type{std::forward<Args>(args)...});
        ++_unsorted_size;
        return {it, true};
    }

    /** Add a value to the set.
     * 
     * If the value is already present, it will not be added again.
     * 
     * @note calls `try_emplace()` with all arguments forwarded.
     * @param Args The arguments to construct the key + value.
     * @return {iterator to element with key, inserted}
     */
    template<typename... Args>
    constexpr std::pair<iterator, bool> emplace(Args&&... args)
    {
        return try_emplace(std::forward<Args>(args)...);
    }
   
    /** Erase element.
     * 
     * @note Calls `consolidate()`.
     * @param it The iterator pointing to the element to be erased.
     * @return The iterator to the position after the element that was removed.
     */
    constexpr const_iterator erase(const_iterator it)
    {
        return _data.erase(it);
    }

    /** Check if the set contains a key and erase if exists.
     * 
     * @note Calls `consolidate()`.
     * @param key The key to remove.
     * @return Number of elements erased (0 or 1).
     */
    [[nodiscard]] constexpr size_type erase(key_type const& key)
    {        
        if (auto const it = lower_bound(key); it != _data.end() and it->key == key) {
            erase(it);
            return 1;
        } else {
            return 0;
        }
    }


private:
    container_type _data = {};
    size_type _unsorted_size = 0;

    /** Get the last position of the sorted range.
     * 
     */
    [[nodiscard]] constexpr auto _middle(this auto&& self) noexcept
    {
        return self.end() - self._unsorted_size;
    }

    /** Search the key using lower bound.
     * 
     * @note Only searches the sorted portion of the map.
     * @param key The key to search
     * @return Iterator to where the key is found, or index where the key should be inserted.
     *         May return _middle() if the key is larger than every key in the map.
     */
    [[nodiscard]] constexpr auto _lower_bound(this auto&& self, key_type const& key)
    {
        return std::lower_bound(self.begin(), self._middle(), key, [](auto const& item, auto const& key) {
            return item.key < key;
        });
    }

    /** Find key in the map.
     * 
     * @param key The key to search
     * @return Iterator inside _data where the key is found, or end()
     */
    [[nodiscard]] constexpr auto _find(this auto&& self, key_type const& key)
    {
        if (auto const it = self._lower_bound(key); it != self._middle() and it->key == key) {
            return it;
        }

        return std::find_if(self._middle(), self.end(), [&key](auto const& item) {
            return item.key == key;
        });
    }
};

}
