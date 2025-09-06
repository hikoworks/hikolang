

#pragma once

#include "path.hpp"
#include "char_category.hpp"
#include <cstddef>
#include <cstdint>

namespace hk {

struct file_location {
    /** The line number in the current source file.
     * 
     * Values:
     *  - 0: The most top line.
     *  - 2**32-1: The maximum tracked line number, or beyond
     *  - other: The line index. 
     */
    std::uint32_t line = 0;

    /** The line number in the upstream source file.
     * 
     * Values:
     *  - 0: The most top line.
     *  - 2**32-1: The maximum tracked line number, or beyond
     *  - other: The column index. 
     */
    std::uint32_t upstream_line = 0;

    /** The utf-8 code-unit number in the current source file.
     *
     * Values:
     *  - 0: The most left column.
     *  - 65535: The maximum tracked column number, or beyond
     *  - other: The column index. 
     */
    std::uint16_t utf8_column = 0;

    /** The utf-16 code-unit number in the current source file.
     *
     * Values:
     *  - 0: The most left column.
     *  - 65535: The maximum tracked column number, or beyond
     *  - other: The column index. 
     */
    std::uint16_t utf16_column = 0;

    /** The utf-32 code-unit number in the current source file.
     *
     * Values:
     *  - 0: The most left column.
     *  - 65535: The maximum tracked column number, or beyond
     *  - other: The column index. 
     */
    std::uint16_t utf32_column = 0;

    /** The upstream source file index.
     * 
     * Values:
     *  - 0: This is the current source file.
     *  - 65535: Unknown file.
     *  - other: The index of the file that generated this file.
     */
    std::uint16_t upstream_file = 0;


    constexpr static std::size_t max_line = std::numeric_limits<uint32_t>::max();
    constexpr static std::size_t max_file = std::numeric_limits<uint16_t>::max();
    constexpr static std::size_t max_column = std::numeric_limits<uint16_t>::max();

    constexpr file_location() noexcept = default;
    constexpr file_location(file_location const&) noexcept = default;
    constexpr file_location(file_location&&) noexcept = default;
    constexpr file_location& operator=(file_location const&) noexcept = default;
    constexpr file_location& operator=(file_location&&) noexcept = default;

    /** Set the line number of the current location.
     *
     * This function is used for the #line directive to set the file and line
     * number for files that where generated from another file.
     *
     * @param line The line number to set. The line number is 0-based, so 0 is
     *             the first line.
     */
    void set_line(std::size_t line) noexcept
    {
        this->upstream_line = line < max_line ? line : max_line;
    }

    /** Set the line number and file of the current location.
     *
     * This function is used for the #line directive to set the file and line
     * number for files that where generated from another file.
     *
     * @note It is UNDEFINED behavior to call this function with a file index
     *       that is greater than max_file.
     * @param file_index The index of the upstream file.
     * @param line The line number to set. The line number is 0-based, so 0 is
     *             the first line.
     */
    void set_line(std::size_t file_index, std::size_t line)
    {
        assert(file_index <= max_file);
        this->upstream_file = file_index;
        set_line(line);
    }

    /** Advance the file location by one code-point.
     *
     * Two code-points are passed to this function, this is used to handle
     * carriage return and line feed characters correctly:
     *  - `\r\n` is treated as a single line ending.
     *  - `\r` is treated as a line ending, but only if it is not followed by a
     *    `\n`.
     *  - `\n` is treated as a line ending.
     *
     * @param cp The code-point that was read.
     * @param cp2 The next code-point that was read, if available.
     */
    void advance(char32_t cp, char32_t cp2) noexcept
    {
        if (is_vertical_space(cp, cp2)) {
            if (line < max_line) {
                ++line;
            }
            if (upstream_line < max_line) {
                ++upstream_line;
            }
            utf8_column = 0;
            utf16_column = 0;
            utf32_column = 0;

        } else {
            // UTF-8, check how many code-units are needed.
            if (cp >= 0x01'0000) {
                utf8_column = utf8_column < max_column - 3 ? utf8_column + 4 : max_column;
            } else if (cp >= 0x0800) {
                utf8_column = utf8_column < max_column - 2 ? utf8_column + 3 : max_column;
            } else if (cp >= 0x0080) {
                utf8_column = utf8_column < max_column - 1 ? utf8_column + 2 : max_column;
            } else {
                utf8_column = utf8_column < max_column ? utf8_column + 1 : max_column;
            }

            // UTF-16, check when surrogate pairs are needed.
            if (cp >= 0x01'0000) {
                utf16_column = utf16_column < max_column - 1 ? utf16_column + 2 : max_column;
            } else {
                utf16_column = utf16_column < max_column ? utf16_column + 1 : max_column;
            }

            utf32_column = utf32_column < max_column ? utf32_column + 1 : max_column;
        }
    }
};

} // namespace hk
