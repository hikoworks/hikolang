

#ifndef HL_LINE_COLUMN_HPP
#define HL_LINE_COLUMN_HPP

#include "char_category.hpp"
#include <cstddef>
#include <utility>

namespace hl {

struct file_offset {
    std::size_t file_id = 0; // Identifier for the file, used to report errors
    std::size_t line = 0;   // 0-based line number
    std::size_t column = 0; // 0-based column number

    constexpr file_offset() noexcept = default;
    constexpr file_offset(file_offset&&) noexcept = default;
    constexpr file_offset(file_offset const&) noexcept = default;
    constexpr file_offset& operator=(file_offset&&) noexcept = default;
    constexpr file_offset& operator=(file_offset const&) noexcept = default;

    constexpr file_offset(std::size_t file_id, std::size_t line = 0, std::size_t column = 0) noexcept
        : file_id(file_id), line(line), column(column) {}

    void advance(char32_t cp) noexcept {
        if (is_vertical_space(cp)) {
            ++line;
            column = 0;
        } else {
            ++column;
        }
    }
};

} // namespace hl
