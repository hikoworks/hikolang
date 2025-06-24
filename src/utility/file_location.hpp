

#pragma once

#include "path.hpp"
#include "char_category.hpp"
#include <cstddef>

namespace hl {

struct file_location {
    /** The path where compilation started.
     */
    hl::path_id base_path_id = hl::path_id::invalid;

    /** The file being compiled.
     */
    hl::path_id path_id = hl::path_id::invalid;

    /** The line being compiled.
     */
    std::size_t line = 0;

    /** The position of the code-point on the line being compiled.
     */
    std::size_t column = 0;

    /** The source file that generated the file being compiled.
     */
    hl::path_id source_path_id = hl::path_id::invalid;

    /** The line in the source file that generated the file being compiled.
     */
    std::size_t source_line = 0;

    constexpr file_location() noexcept = default;
    constexpr file_location(file_location const&) noexcept = default;
    constexpr file_location(file_location&&) noexcept = default;
    file_location& operator=(file_location const&) noexcept = default;
    file_location& operator=(file_location&&) noexcept = default;

    /** Create a file_location with the given base path and path.
     * 
     * @param base_path_id The path where compilation started.
     * @param path_id The file being compiled.
     */
    constexpr file_location(hl::path_id base_path_id, hl::path_id path_id) noexcept
        : base_path_id(base_path_id), path_id(path_id), line(0), column(0),
          source_path_id(path_id), source_line(0)
    {
    }

    /** Set the line number of the current location.
     * 
     * This function is used for the #line directive to set the file and line number
     * for files that where generated from another file.
     * 
     * @param line The line number to set. The line number is 0-based, so 0 is the first line.
     */
    void set_line(std::size_t line) noexcept
    {
        this->source_line = line;
        this->column = 0;
    }

    /** Set the line number and file of the current location.
     * 
     * This function is used for the #line directive to set the file and line number
     * for files that where generated from another file.
     * 
     * @param path The path of the file to set. This is relative to the source path.
     *             This file does not need to exist.
     * @param line The line number to set. The line number is 0-based, so 0 is the first line.
     */
    void set_line(std::filesystem::path const& path, std::size_t line)
    {
        this->source_path_id = hl::get_path_id(path, this->path_id);
        set_line(line);
    }

    /** Advance the file location by one code-point.
     * 
     * Two code-points are passed to this function, this is used to handle carriage return
     * and line feed characters correctly:
     *  - `\r\n` is treated as a single line ending.
     *  - `\r` is treated as a line ending, but only if it is not followed by a `\n`.
     *  - `\n` is treated as a line ending.
     * 
     * @param cp The code-point that was read.
     * @param cp2 The next code-point that was read, if available.
     */
    void advance(char32_t cp, char32_t cp2) noexcept
    {
        if (is_vertical_space(cp, cp2)) {
            ++line;
            ++source_line;
            column = 0;

        } else {
            ++column;
        }
    }
};

} // namespace hl
