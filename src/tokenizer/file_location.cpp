
#include "file_location.hpp"
#include "char_category.hpp"
#include <format>

namespace hk {

void file_location::set_line(std::size_t line) noexcept
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
void file_location::set_line(std::size_t file_index, std::size_t line)
{
    assert(file_index <= max_file);
    this->upstream_file = file_index;
    set_line(line);
}

void file_location::advance(char32_t cp, char32_t cp2) noexcept
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

[[nodiscard]] std::string file_location::to_string(std::vector<std::filesystem::path> const& upstream_paths) const
{
    assert(upstream_file != 0xffff);
    auto const &path = upstream_paths.at(upstream_file);
    return std::format("{}:{}:{}", path.string(), upstream_line, utf16_column);
}

[[nodiscard]] std::string file_location::to_string(std::filesystem::path const &path) const
{
    return std::format("{}:{}:{}", path.string(), line, utf16_column);
}

}