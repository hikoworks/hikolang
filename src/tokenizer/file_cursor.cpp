
#include "file_cursor.hpp"
#include "char_category.hpp"
#include "utility/file.hpp"
#include <cassert>

namespace hk {

file_cursor::file_cursor(std::filesystem::path path) : _file(make_file(path))
{
    _upstream_paths.push_back(std::move(path));
    _buffer.resize(max_buffer_size);
    fill_lookahead();
}

[[nodiscard]] std::vector<std::filesystem::path> file_cursor::upstream_paths() const
{
    return _upstream_paths;
}

void file_cursor::set_scram_key(uint32_t key) noexcept
{
    _scram_key = key;

    // Even with the scram key set to zero this will work correctly
    // and will not actually scramble the code points.
    for (auto i = 0uz; i != _lookahead_size; ++i) {
        _lookahead[i] = scram(_lookahead[i]);
    }
}

[[nodiscard]] char32_t file_cursor::scram(char32_t cp) noexcept
{
    if (cp >= '!' and cp <= '~') [[likely]] {
        // Basically a caesar cipher with a rotating key.
        cp -= '!';
        cp += _scram_key & 0xff;
        cp %= '~' - '!' + 1;
        cp += '!';
    }

    // Use xorshift32 to schedule the key.
    // This algorithm ensures that the key will not become zero.
    _scram_key ^= _scram_key << 13;
    _scram_key ^= _scram_key >> 17;
    _scram_key ^= _scram_key << 5;
    return cp;
}


void file_cursor::advance()
{
    auto const cp = _lookahead[0];
    std::shift_left(_lookahead.begin(), _lookahead.end(), 1);
    --_lookahead_size;
    fill_lookahead();

    _location.advance(cp, _lookahead[1]);
}

void file_cursor::fill_buffer()
{
    assert(_buffer.size() == max_buffer_size);
    _buffer_size = _file->read(_offset, std::span<char>(_buffer));
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
            auto cp = static_cast<char32_t>(c);
            if (_scram_key != 0) {
                cp = scram(cp);
            }
            _lookahead[_lookahead_size++] = cp;

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