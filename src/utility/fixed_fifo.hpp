
#pragma once

#include <array>
#include <cstddef>
#include <algorithm>
#include <cassert>
#include <utility>
#include <memory>

namespace hl {

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
        assert(not empty());
        return _data[0].value;
    }

    /** Get the first element in the fifo.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function if the queue is empty.
     * @return A reference to the first element in the queue.
     */
    [[nodiscard]] constexpr value_type& front()
    {
        assert(not empty());
        return _data[0].value;
    }

    /** Get the last element in the fifo.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function if the queue is empty.
     * @return A const reference to the last element in the queue.
     */
    [[nodiscard]] constexpr value_type const& back() const
    {
        assert(not empty());
        return _data[_size - 1].value;
    }

    /** Get the last element in the fifo.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function if the queue is empty.
     * @return A reference to the last element in the queue.
     */
    [[nodiscard]] constexpr value_type& back()
    {
        assert(not empty());
        return _data[_size - 1].value;
    }

    /** Access an element at a specific index in the fifo.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function with an index that is out of bounds.
     * @param i The index of the element to access.
     * @return A const reference to the element at the specified index.
     */
    [[nodiscard]] constexpr value_type const& operator[](size_type i) const
    {
        assert(i < size());
        return _data[i].value;
    }

    /** Access an element at a specific index in the fifo.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function with an index that is out of bounds.
     * @param i The index of the element to access.
     * @return A reference to the element at the specified index.
     */
    [[nodiscard]] constexpr value_type& operator[](size_type i)
    {
        assert(i < size());
        return _data[i].value;
    }

    /** Clear the fifo, removing all elements.
     * 
     * This function resets the size of the queue to 0, effectively clearing it.
     */
    constexpr void clear() noexcept
    {
        for (auto i = 0uz; i != _size; ++i) {
            std::destroy_at(std::addressof(_data[i].value));
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
        std::construct_at(std::addressof(_data[_size].value), std::forward<Args>(args)...);
        return _data[_size++].value;
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
        std::construct_at(std::addressof(_data[_size].value), value);
        return _data[_size++].value;
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
        std::construct_at(std::addressof(_data[_size].value), std::move(value));
        return _data[_size++].value;
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

        auto tmp = std::move(_data[i].value);
        for (; i != _size - 1; ++i) {
            _data[i].value = std::move(_data[i + 1].value);
        }
        std::destroy_at(std::addressof(_data[--_size].value));
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
    union element_type {
        constexpr element_type() : monostate{} {}
        constexpr ~element_type() {}

        std::monostate monostate;
        value_type value;
    };

    size_type _size = 0;
    element_type _data[Size];
};

} // namespace hl
