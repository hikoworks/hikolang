
#pragma once

#include <cstdint>
#include <cstddef>
#include <vector>
#include <mutex>
#include <algorithm>
#include <cassert>
#include <memory>

namespace hk {


/** Holds interned values.
 * 
 * 
 * @tparam T The type of the value to intern. must be able to be compared with `<` and `==`.
 */
template<typename T>
class interned {
public:
    using value_type = T;

    /** Number of objects that are currently interned.
     */
    [[nodiscard]] static std::size_t num_interned() noexcept
    {
        std::unique_lock<std::mutex> lock{_mutex};
        return _object_to_index.size();
    }

    constexpr interned() noexcept = default;
    constexpr interned(interned const &) noexcept = default;
    constexpr interned(interned &&) noexcept = default;
    constexpr interned &operator=(interned const &) noexcept = default;
    constexpr interned &operator=(interned &&) noexcept = default;
    constexpr ~interned() noexcept = default;

    interned(value_type const &value) requires(std::is_copy_constructible_v<value_type>) :
        _ptr(insert(std::addressof(value)))
    {
    }

    /** Reset this object to an empty state.
     */
    constexpr void reset() noexcept
    {
        _ptr = nullptr;
    }

    /** Assign a new value to this object.
     * 
     * @param value The value to assign.
     */
    interned &operator=(value_type const &value) requires(std::is_copy_assignable_v<value_type>)
    {
        _ptr = insert(std::addressof(value));
        return *this;
    }

    /** Automatically convert to the value interned.
     * 
     * This allows you to use the interned object as if it were the value type.
     * @note It is UNDEFINED BEHAVIOR to use this operator if the interned object is empty.
     * @return A reference to the interned value.
     */
    operator value_type const &() const
    {
        assert(_ptr != nullptr);
        return *_ptr;
    }

    [[nodiscard]] constexpr friend bool operator==(interned const&, interned const&) = default;

    [[nodiscard]] constexpr friend auto operator<=>(interned const& lhs, interned const& rhs)
    {
        if (lhs._ptr == nullptr and rhs._ptr == nullptr) {
            return std::strong_ordering::equal;
        } else if (lhs._ptr == nullptr) {
            return std::strong_ordering::less;
        } else if (rhs._ptr == nullptr) {
            return std::strong_ordering::greater;
        }

        return *lhs._ptr <=> *rhs._ptr;
    }

    /** Check if this interned object is equal to a value.
     * 
     * This allows you to compare the interned object with a value of the same type.
     * 
     * @param lhs The interned object to compare.
     * @param rhs The value to compare with.
     * @return true if the interned object is equal to the value, false otherwise.
     */
    [[nodiscard]] constexpr friend bool operator==(interned const &lhs, value_type const &rhs)
    {
        if (lhs._ptr == nullptr) {
            return false;
        }
        return *lhs._ptr == rhs;
    }

    /** Check if this interned object is not empty.
     * 
     * This allows you to check if the interned object contains a valid pointer to an interned object.
     * 
     * @return true if the interned object is not empty, false otherwise.
     */
    [[nodiscard]] explicit operator bool() const noexcept
    {
        return _ptr != nullptr;
    }

    /** Get a pointer to the interned object.
     * 
     * This allows you to access the interned object as a pointer.
     * 
     * @return A pointer to the interned object, or nullptr if it is empty.
     */
    [[nodiscard]] value_type const *get() noexcept
    {
        return _ptr;
    }

    /** Dereference the interned object.
     * 
     * This allows you to access the interned object as if it were a reference.
     * 
     * @note It is UNDEFINED BEHAVIOR to dereference this pointer if the interned object is empty.
     * @return A reference to the interned object.
     */
    [[nodiscard]] value_type const &operator*() const
    {
        assert(_ptr != nullptr);
        return *_ptr;
    }

    /** Derefernce the interned object.
     * 
     * This allows you to access the interned object as if it were a pointer.
     * 
     * @note It is UNDEFINED BEHAVIOR to dereference this pointer if the interned object is empty.
     * @return A pointer to the interned object.
     */
    [[nodiscard]] value_type const *operator->() const
    {
        assert(_ptr != nullptr);
        return _ptr;
    }


private:
    static inline std::mutex _mutex;
    static inline std::vector<value_type const*> _object_to_index;

    value_type const *_ptr = nullptr;

    [[nodiscard]] value_type const *insert(value_type const* ptr)
    {
        auto const _ = std::unique_lock{_mutex};

        auto const it = std::lower_bound(_object_to_index.begin(), _object_to_index.end(), ptr, [](auto const& a, auto const& b) {
            return *a < *b;
        });

        if (it != _object_to_index.end() and **it == *ptr) {
            // The object is already interned, return the existing pointer.
            return *it;
        }

        // Make a copy of the value to intern.
        ptr = new value_type{*ptr};
        _object_to_index.insert(it, ptr);
        return ptr;
    }
};

}