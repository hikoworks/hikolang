
#include "cursor.hpp"
#include "file.hpp"
#include <cassert>

namespace hl {

cursor::cursor(hl::path_id path_id) noexcept : _path_id(path_id), _source_path_id(path_id)
{
    _buffer.resize(max_buffer_size);
    fill_lookahead();
}

void set_line(size_t line) noexcept
{
    _source_line = line;
}

void cursor::set_line(std::filesystem::path const& path, size_t line)
{
    _source_path_id = hl::get_path_id(std::move(path), _path_id);
    set_line(line);
}

void cursor::advance()
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

void cursor::fill_buffer()
{
    auto file = hl::get_file(_path_id);

    assert(_buffer.size() == max_buffer_size);
    _buffer_size = file->read(_offset, std::span<char>(_buffer));
    if (_buffer_size < max_buffer_size) {
        // Fill the rest of the buffer with zeros, so that the lookahead
        // can safely read beyond the end of the file.
        std::fill(_buffer.begin() + _buffer_size, _buffer.end(), 0);
    }
}

void cursor::fill_lookahead()
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