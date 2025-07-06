
#pragma once

#include <array>
#include <cstddef>
#include <algorithm>
#include <cassert>
#include <utility>
#include <memory>

namespace hk {

/** A fixed-size FIFO queue implementation.
 * 
 * This class provides a fixed-size FIFO queue that can be used to store elements
 * of type T.
 * 
 * @tparam T The type of the elements stored in the queue.
 * @tparam Size The maximum number of elements the queue can hold.
 */
template<typename T, std::size_t Size>
class fixed_fifo {
public:
    /** Type alias for the value type stored in the fifo.
     * 
     * This is the type of elements that can be stored in the fifo.
     */
    using value_type = T;

    /** Type alias for the size type of the fifo.
     * 
     * This is typically std::size_t, representing the number of elements in the fifo.
     */
    using size_type = std::size_t;

    /** Destructor.
     * 
     * Cleans up the fifo by destroying all elements in it.
     */
    constexpr ~fixed_fifo()
    {
        clear();
    }

    /** Default constructor.
     * 
     * Initializes the FIFO queue with a size of 0.
     */
    constexpr fixed_fifo() noexcept = default;

    /** Copy constructor.
     * 
     * @note This is deleted to prevent copying of the fifo.
     */
    constexpr fixed_fifo(fixed_fifo const&) = delete;

    /** Move constructor.
     * 
     * @note This is deleted to prevent moving of the fifo.
     */
    constexpr fixed_fifo(fixed_fifo&&) = delete;

    /** Copy assignment operator.
     * 
     * @note This is deleted to prevent copying of the fifo.
     */
    constexpr fixed_fifo& operator=(fixed_fifo const&) = delete;

    /** Move assignment operator.
     * 
     * @note This is deleted to prevent moving of the fifo.
     */
    constexpr fixed_fifo& operator=(fixed_fifo&&) = delete;

    /** The number of elements in the fifo.
     * 
     * @return The current size of the queue.
     */
    [[nodiscard]] constexpr size_type size() const noexcept
    {
        return _size;
    }

    /** The maximum number of elements the fifo can hold.
     * 
     * @return The maximum size of the queue.
     */
    [[nodiscard]] constexpr size_type max_size() const noexcept
    {
        return Size;
    }

    /** Check if the fifo is empty.
     * 
     * @return true if the queue is empty, false otherwise.
     */
    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    /** Check if the fifo is full.
     * 
     * @return true if the queue is full, false otherwise.
     */
    [[nodiscard]] constexpr bool full() const noexcept
    {
        return size() == max_size();
    }

    /** Get the first element in the fifo.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function if the queue is empty.
     * @return A const reference to the first element in the queue.
     */
    [[nodiscard]] constexpr value_type const& front() const
    {
        return _data.front();
    }

    /** Get the first element in the fifo.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function if the queue is empty.
     * @return A reference to the first element in the queue.
     */
    [[nodiscard]] constexpr value_type& front()
    {
        return _data.front();
    }

    /** Get the first element in the fifo.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function if the queue is empty.
     * @return A const reference to the first element in the queue.
     */
    [[nodiscard]] constexpr value_type const& back() const
    {
        assert(not empty());
        return _data[size() - 1];
    }

    /** Get the first element in the fifo.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function if the queue is empty.
     * @return A reference to the first element in the queue.
     */
    [[nodiscard]] constexpr value_type& back()
    {
        assert(not empty());
        return _data[size() - 1];
    }

    /** Access an element at a specific index in the fifo.
     * 
     * You are allowed to access elements in the fifo beyond the current size, up to the maximum size.
     * This would return a reference to the element with the default constructed value type.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function with an index that is out of bounds of the
     *       of the internal array.
     * @param i The index of the element to access.
     * @return A const reference to the element at the specified index.
     */
    [[nodiscard]] constexpr value_type const& operator[](size_type i) const
    {
        assert(i < max_size());
        return _data[i];
    }

    /** Access an element at a specific index in the fifo.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function with an index that is out of bounds.
     * @param i The index of the element to access.
     * @return A reference to the element at the specified index.
     */
    [[nodiscard]] constexpr value_type& operator[](size_type i)
    {
        assert(i < max_size());
        return _data[i];
    }

    /** Clear the fifo, removing all elements.
     * 
     * This function resets the size of the queue to 0, effectively clearing it.
     */
    constexpr void clear() noexcept
    {
        for (auto i = 0uz; i != _size; ++i) {
            _data[i] = value_type{};
        }
        _size = 0;
    }

    /** Emplace an element to the back of the queue.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function if the queue is full.
     * @param args The arguments to forward to the constructor of T.
     * @return A reference to the newly emplaced element.
     */
    template<typename... Args>
    constexpr value_type& emplace(Args&&... args)
    {
        assert(not full());
        _data[_size] = value_type{std::forward<Args>(args)...};
        return _data[_size++];
    }

    /** Push an element to the back of the queue.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function if the queue is full.
     * @param value The value to push to the back of the queue.
     * @return A reference to the newly pushed element.
     */
    constexpr value_type& push_back(value_type const& value)
    {
        assert(not full());
        _data[_size] = value;
        return _data[_size++];
    }

    /** Push an element to the back of the queue.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function if the queue is full.
     * @param value The value to push to the back of the queue.
     * @return A reference to the newly pushed element.
     */
    constexpr value_type& push_back(value_type&& value)
    {
        assert(not full());
        _data[_size] = std::move(value);
        return _data[_size++];
    }

    /** Pop an element from the queue.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function if the queue is empty.
     * @note It is UNDEFINED BEHAVIOR to call this function with an index that is out of bounds.
     * @param i The index of the element to pop/remove, defaults to 0 (the front).
     * @return The element at the front of the queue.
     */
    constexpr value_type remove(size_type i)
    {
        assert(not empty());
        assert(i < size());

        auto tmp = std::move(_data[i]);
        for (; i != _size - 1; ++i) {
            _data[i] = std::move(_data[i + 1]);
        }
        _data[--_size] = value_type{};
        return tmp;
    }

    /** Pop the first element from the queue.
     * 
     * This is equivalent to calling remove(0).
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function if the queue is empty.
     * @return The first element in the queue.
     */
    constexpr value_type pop_front()
    {
        return remove(0);
    }

    /** Pop the last element from the queue.
     * 
     * This is equivalent to calling remove(size() - 1).
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function if the queue is empty.
     * @return The last element in the queue.
     */
    constexpr value_type pop_back()
    {
        return remove(_size - 1);
    }

private:
    size_type _size = 0;
    std::array<value_type, Size> _data = {};
};

} // namespace hk
