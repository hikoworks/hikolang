
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

    void pop_front() noexcept
    {
        if (_size > 0) {
            std::shift_left(_values.begin(), _values.end(), 1);
            _values[_values.size() - 1] = {};
            --_size;
        }
    }

    void push_back(char const* start, char32_t cp) noexcept
    {
        assert(start != nullptr);

        if (_size == _values.size()) {
            // If we are full, pop the front value.
            pop_front();
        }
        _values[_size++] = {start, cp};
    }

    bool decode_utf8(char const*& ptr, char const* const end) noexcept
    {
        assert(ptr != nullptr);
        assert(end != nullptr);

        while (ptr != end and _size != _values.size()) {
            auto const prev_ptr = ptr;
            auto const cp = decode_utf8_code_point(ptr, end);
            push_back(prev_ptr, cp);
        }
        return _size != 0;
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
