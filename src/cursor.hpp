
#ifndef HL_CURSOR_HPP
#define HL_CURSOR_HPP

#include "utf8.hpp"
#include <cstddef>


namespace hl {

class cursor {
public:
    constexpr cursor() noexcept = default;
    constexpr cursor(std::size_t file_id) noexcept : _file_id(file_id), _source_file_id(file_id) {}

    constexpr void set_line(size_t line) noexcept
    {
        _source_line = line;
    }

    /** Synchronize the cursor with the source file.
     * 
     * This function should be called after advancing beyond the vertical space
     * after the #line directive.
     */
    constexpr void set_line(std::string file_name, size_t line) noexcept;

    constexpr void set_line(std::size_t line) noexcept
    {
        _line = line;
        _source_line = line;
        _column = 0;
    }

    constexpr void advance(char32_t cp) noexcept
    {
        if (is_vertical_space(cp)) {
            ++_line;
            ++_source_line;
            _column = 0;
        } else {
            ++_column;
        }
    }

private:
    std::size_t _file_id = 0;
    std::size_t _offset = 0;
    std::size_t _line = 0;
    std::size_t _column = 0;

    std::size_t _source_file_id = 0;
    std::size_t _source_line = 0;
};

}

#endif