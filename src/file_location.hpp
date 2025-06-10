

#ifndef HL_FILE_LOCATION_HPP
#define HL_FILE_LOCATION_HPP

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

    constexpr file_location(hl::path_id base_path_id, hl::path_id path_id) noexcept
        : base_path_id(base_path_id), path_id(path_id), line(0), column(0),
          source_path_id(path_id), source_line(0)
    {
    }

    void set_line(std::size_t line) noexcept
    {
        this->source_line = line;
        this->column = 0;
    }

    void set_line(std::filesystem::path const& path, std::size_t line)
    {
        this->source_path_id = hl::get_path_id(path, this->path_id);
        set_line(line);
    }

    void advance(char32_t cp) noexcept
    {
        if (is_vertical_space(cp)) {
            ++line;
            ++source_line;
            column = 0;
        } else {
            ++column;
        }
    }
};

} // namespace hl

#endif // HL_FILE_LOCATION_HPP
