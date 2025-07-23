
#pragma once

#include "path.hpp"
#include "file_location.hpp"
#include "file.hpp"
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

namespace hk {

/** A cursor pointing into a file, being able to read linearly from the file.
 * 
 */
class file_cursor {
public:
    /** Create a file_cursor that points to the given file.
     * 
     * @param path The path of the file being read.
     */
    file_cursor(std::filesystem::path path);

    /** Get the file location where this file_cursor is currently pointint to.
     * 
     * @return The file_location of the current position in the file.
     */
    [[nodiscard]] file_location location() const noexcept
    {
        return _location;
    }

    /** Set the line number of the current location.
     * 
     * @param line The line number to set. The line number is 0-based, so 0 is the first line.
     */
    void set_line(size_t line) noexcept
    {
        return _location.set_line(line);
    }

    /** Synchronize the file_cursor with the source file.
     * 
     * The vertical space after the #line directive must be consumed before calling this function.
     * 
     * @param file_name The name of the file to set.
     * @param line The line number to set. The line number is 0-based, so 0 is the first line.
     */
    void set_line(std::filesystem::path file_name, size_t line)
    {
        auto it = std::find(_upstream_paths.begin(), _upstream_paths.end(), file_name);
        if (it != _upstream_paths.end()) {
            // Found the path.
            auto const file_index = std::distance(_upstream_paths.begin(), it);
            return _location.set_line(file_index, line);
        }

        if (_upstream_paths.size() == file_location::max_file - 2) {
            // Too many files, but continue anyway.
            _upstream_paths.emplace_back("<unknown file>");
            return _location.set_line(file_location::max_file - 1);
        }

        _upstream_paths.push_back(std::move(file_name));
        return _location.set_line(_upstream_paths.size() - 1, line);
    }

    /** Get the paths that where used to generate the file being parsed.
     * 
     * The first path in the vector is the file that is being compiled.
     * Subsequent paths are the paths of files that where used to generate the
     * file ab 
     * @return A vector of paths that where used to generate the file being parsed.
     */
    [[nodiscard]] std::vector<std::filesystem::path> upstream_paths() const;

    /** Set the key used for the #scram directive.
     * 
     * This key is used to scramble the file contents.
     * 
     * @param key The key to set.
     */
    void set_scram_key(uint32_t key) noexcept;

    /** Advance the file_cursor to the next code-point in the file.
     */
    void advance();

    /** Advance the file_cursor to the next code-point in the file.
     */
    file_cursor &operator++()
    {
        advance();
        return *this;
    }

    /** Advance the file_cursor to the next code-point in the file.
     * 
     * @param n The number of code-points to advance.
     */
    file_cursor &operator+=(unsigned int n)
    {
        for (unsigned int i = 0; i != n; ++i) {
            advance();
        }
        return *this;
    }
    
    /** Get the code-point at the given offset in the lookahead buffer.
     * 
     * You may lookahead up to 8 code-points in the file.
     * 
     * If you lookeahead beyond the end of the file, the code-point will be 0.
     * This makes it much easier to write parsers as end-of-file does not
     * need to be continuesly checked.
     * 
     * @param offset The offset in the lookahead buffer to read from.
     * @return The code-point at the given offset in the lookahead buffer.
     */
    [[nodiscard]] char32_t operator[](std::size_t offset) const noexcept
    {
        assert(offset < _lookahead.size());
        return _lookahead[offset];
    }

    /** The number of code-points in the lookahead buffer.
     * 
     * Only when the lookahead buffer is beyond the end of the file this will be less than 8.
     * 
     * @return The number of code-points in the lookahead buffer.
     */
    [[nodiscard]] std::size_t size() const noexcept
    {
        return _lookahead_size;
    }

    /** Check if all code-points have been read/advanced in the file.
     * 
     * @retval true if the end of the file has been reached.
     */
    [[nodiscard]] bool end_of_file() const noexcept
    {
        return size() == 0;
    }
    
private:
    /** The maximum size of the buffer used to read the file.
     */
    constexpr static std::size_t max_buffer_size = 4096;

    constexpr static std::size_t buffer_mask = max_buffer_size - 1;

    /** The file object to read from.
     */
    std::unique_ptr<file> _file;

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

    /** A list of upstream files.
     * 
     * Entries:
     *  - 0: The current file.
     *  - file_location::max_file: The last valid file.
     */
    std::vector<std::filesystem::path> _upstream_paths = {};

    /** The key used for the #scram directive.
     * 
     * The key is set by the #scram directive and is used to scramble the file contents.
     */
    uint32_t _scram_key = 0;

    /** Fill the buffer with the next chunk of the file.
     * 
     * When the file does not have enough data to fill the buffer,
     * the rest of the buffer will be filled with zeros.
     */
    void fill_buffer();

    /** Fill the lookahead buffer with the next code-points from the file.
     * 
     * This will call fill_buffer() automatically when it will need more bytes to read.
     */
    void fill_lookahead();

    /** Scramble a code-point using the #scram directive key.
     * 
     * If the key is zero, this function will return the code-point unchanged.
     * And it will not advance the key. However it is recommended to skip
     * the scram function of the key is zero for performance reasons.
     * 
     * @param cp The code-point to scramble.
     * @return The scrambled code-point.
     */
    [[nodiscard]] char32_t scram(char32_t cp) noexcept;
};

}
