
#include "file_cursor.hpp"
#include "file.hpp"
#include "char_category.hpp"
#include <cassert>

namespace hl {

file_cursor::file_cursor(hl::path_id base_path_id, hl::path_id path_id) noexcept : _location(base_path_id, path_id)
{
    _buffer.resize(max_buffer_size);
    fill_lookahead();
}

void file_cursor::advance()
{
    auto const cp = _lookahead[0];
    std::shift_left(_lookahead.begin(), _lookahead.end(), 1);
    --_lookahead_size;
    fill_lookahead();

    _location.advance(cp);
}

void file_cursor::fill_buffer()
{
    auto &file = hl::get_file(_location.path_id);

    assert(_buffer.size() == max_buffer_size);
    _buffer_size = file.read(_offset, std::span<char>(_buffer));
    if (_buffer_size < max_buffer_size) {
        // Fill the rest of the buffer with zeros, so that the lookahead
        // can safely read beyond the end of the file.
        std::fill(_buffer.begin() + _buffer_size, _buffer.end(), 0);
    }
}

void file_cursor::fill_lookahead()
{
    char32_t code_point = 0;
    std::size_t code_units_left = 0;

    while (_lookahead_size != _lookahead.size()) {
        auto const buffer_offset = _offset & buffer_mask;
        if (buffer_offset == 0) {
            fill_buffer();
        }

        if (buffer_offset >= _buffer_size) {
            return;
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
}