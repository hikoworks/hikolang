
#ifndef HL_CURSOR_HPP
#define HL_CURSOR_HPP

#include "utf8.hpp"
#include "path.hpp"
#include "file_location.hpp"
#include <cstddef>
#include <bit>
#include <vector>
#include <array>
#include <span>
#include <cassert>
#include <string>
#include <string_view>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <format>
#include <algorithm>
#include <filesystem>

namespace hl {

class file_cursor {
public:
    constexpr file_cursor() noexcept = default;

    file_cursor(hl::path_id base_path_id, hl::path_id path_id) noexcept;

    [[nodiscard]] file_location location() const noexcept
    {
        return _location;
    }

    void set_line(size_t line) noexcept
    {
        return _location.set_line(line);
    }

    /** Synchronize the file_cursor with the source file.
     * 
     * This function should be called after advancing beyond the vertical space
     * after the #line directive.
     */
    void set_line(std::filesystem::path const& file_name, size_t line)
    {
        return _location.set_line(file_name, line);
    }

    void advance();

    file_cursor &operator++()
    {
        advance();
        return *this;
    }

    file_cursor &operator+=(unsigned int n)
    {
        for (unsigned int i = 0; i != n; ++i) {
            advance();
        }
        return *this;
    }
    
    [[nodiscard]] char32_t operator[](std::size_t offset) const noexcept
    {
        assert(offset < _lookahead_size);
        return _lookahead[offset];
    }

    [[nodiscard]] std::size_t size() const noexcept
    {
        return _lookahead_size;
    }

    [[nodiscard]] bool end_of_file() const noexcept
    {
        return _lookahead_size == 0;
    }

    
private:
    /** The maximum size of the buffer used to read the file.
     */
    constexpr static std::size_t max_buffer_size = 4096;

    constexpr static std::size_t buffer_mask = max_buffer_size - 1;

    /** The buffer used to read the file.
     * 
     * This buffer is used to read the file in chunks, so that we can efficiently
     * read the file without having to read it character by character.
     */
    std::vector<char> _buffer = {};

    /** The number of bytes that where read from the file.
     * 
     * If less than max_buffer_size, the end of the file has been reached.
     */
    std::size_t _buffer_size = 0;

    /** The offset in the file where the next code-point will be read from in the the lookahead buffer.
     */
    std::size_t _offset = 0;

    /** The code-point lookahead buffer.
     */
    std::array<char32_t, 8> _lookahead = {};

    /** The number of characters in the lookahead buffer.
     * 
     * If this is zero, the end of the file has been reached.
     */
    std::size_t _lookahead_size = 0;

    /** The file location where the current first character is positioned.
     */
    file_location _location = {};

    void fill_buffer();

    void fill_lookahead();
};

}

#endif