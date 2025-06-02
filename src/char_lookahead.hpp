
#ifndef HL_CHAR_LOOKAHEAD_HPP
#define HL_CHAR_LOOKAHEAD_HPP

#include "utf8.hpp"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <algorithm>

namespace hl {

template<size_t N>
class character_lookahead {
public:
    struct value_type {
        char const* start = nullptr;
        char32_t cp = 0;
    };

    [[nodiscard]] std::size_t size() const noexcept
    {
        return _size;
    }

    [[nodiscard]] bool empty() const noexcept
    {
        return _size == 0;
    }

    [[nodiscard]] constexpr std::size_t capacity() const noexcept
    {
        return _values.size();
    }

    [[nodiscard]] constexpr bool full() const noexcept
    {
        return _size == capacity();
    }

    void pop_front() noexcept
    {
        if (not empty()) {
            std::shift_left(_values.begin(), _values.end(), 1);
            _values[_values.size() - 1] = {};
            --_size;
        }
    }

    void push_back(char const* start, char32_t cp) noexcept
    {
        assert(start != nullptr);

        if (full()) {
            // If we are full, pop the front value.
            pop_front();
        }
        _values[_size++] = {start, cp};
    }

    /** Decode UTF-8 encoded Unicode code-points until full.
     * 
     * This function will decode UTF-8 encoded Unicode code-points from the input
     * buffer until the lookahead buffer is full or the end of the input is reached.
     * 
     * If the end of the input is reached, code-points in the lookahead buffer
     * will be set to zero. However the .start pointer will be set to nullptr.
     * 
     * Errors in the UTF-8 encoding will be handled by placing special values
     * in the code-point field of the lookahead buffer. These values are defined
     * in the `unicode_error` enum in `utf8.hpp`.
     * 
     * @param ptr Pointer to the current position in the input buffer.
     * @param end Pointer to the end of the input buffer.
     */
    void decode_utf8(char const*& ptr, char const* const end) noexcept
    {
        assert(ptr != nullptr);
        assert(end != nullptr);

        while (ptr != end and not full()) {
            auto const prev_ptr = ptr;
            auto const cp = decode_utf8_code_point(ptr, end);
            push_back(prev_ptr, cp);
        }
    }

    [[nodiscard]] constexpr value_type const& operator[](std::size_t index) const noexcept
    {
        assert(index < _values.size());
        return _values[index];
    }

private:
    std::array<value_type, N> _values = {};
    std::size_t _size = 0;
};

}

#endif // HL_CHAR_LOOKAHEAD_HPP
