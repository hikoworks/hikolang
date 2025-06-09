
#ifndef HL_CURSOR_HPP
#define HL_CURSOR_HPP

#include "utf8.hpp"
#include "path.hpp"
#include <cstddef>
#include <bit>

namespace hl {

class cursor {
public:
    constexpr cursor() noexcept = default;

    constexpr cursor(hl::path_id path_id) noexcept : _path_id(path_id), _source_path_id(path_id)
    {
        _buffer.resize(buffer_size);
        fill_lookahead();
    }

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

    constexpr void advance() noexcept
    {
        auto const cp = _lookahead[0];
        std::shift_left(_lookahead.begin(), _lookahead.end(), 1);
        --_lookahead_size;
        fill_lookahead();

        if (is_vertical_space(cp)) {
            ++_line;
            ++_source_line;
            _column = 0;
        } else {
            ++_column;
        }
    }

private:
    /** The buffer contain two windows of 4096 bytes each.
     * 
     * When `_offset` is beyond the zero byte of a window, the other
     * window is filled with the next 4096 bytes from the file.
     * To do a byte lookehead we can simply use an AND operation on `_offset`.
     */
    constexpr static std::size_t buffer_size = 4096;
    std::vector<char> _buffer;
    std::size_t _offset = 0;

    std::array<char32_t, 8> _lookahead;
    std::size_t _lookahead_size = 0;

    hl::path_id _path_id = hl::path_id::invalid;
    std::size_t _line = 0;
    std::size_t _column = 0;

    hl::path_id _source_path_id = hl::path_id::invalid;
    std::size_t _source_line = 0;

    void fill_buffer()
    {
        auto file = hl::get_file(_path_id);

        assert(_buffer.size() == buffer_size);
        auto const read_size = file->read(_offset, std::span<char>(_buffer));
        if (read_size < buffer_size) {
            // Fill the rest of the buffer with zeros, so that the lookahead
            // can safely read beyond the end of the file.
            std::fill(_buffer.begin() + read_size, _buffer.end(), 0);
        }
    }

    void fill_lookahead()
    {
        char32_t code_point = 0;
        std::size_t code_units_left = 0;

        while (_lookahead_size != _lookahead.size()) {
            auto const buffer_offset = _offset & 0xFFF; // 4096 bytes window
            if (buffer_offset == 0) {
                fill_buffer();
            }
            ++_offset;

            auto c = static_cast<uint8_t>(_buffer[buffer_offset]);
            if (c < 0x80) [[likely]] {
                // Fast path for ASCII characters
                _lookahead[_lookahead_size++] = static_cast<char32_t>(c);

            } else if ((c & 0xc0) == 0x80 and code_units_left != 0) {
                // Valid continuation byte.
                code_point <<= 6;
                code_point |= (c & 0x3f);
                if (--code_units_left == 0) {
                    _lookahead[_lookahead_size++] = code_point;
                    code_point = 0;
                }

            } else if ((c & 0xc0) != 0x80 and code_units_left == 0) {
                // Start of a new UTF-8 multi-byte sequence.
                code_units_left = std::countl_one(c);

                assert(code_units_left != 0);
                if (code_units_left > 4) [[unlikely]] {
                    // Invalid UTF-8 multi-byte sequence, more than 4 bytes.
                    _lookahead[_lookahead_size++] = 0xfffd;
                    code_units_left = 0;
                    continue;
                }

                // Strip the leading bits
                c <<= code_units_left;
                c >>= code_units_left;
                code_point = c;
                --code_units_left;

            } else {
                // Unexpected byte in UTF-8 sequence.
                _lookahead[_lookahead_size++] = 0xfffd;
                code_units_left = 0;
            }
        }
    }
};

}

#endif