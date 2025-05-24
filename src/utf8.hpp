
#include <iterator>
#include <cstdint>
#include <expected>
#include <cstddef>
#include <cassert>
#include <utility>
#include <array>
#include <string>

namespace hl {

/** Decode a single Unicode code-point from a UTF-8 stream.
 *
 * RFC 3629 recommends that invalid UTF-8 sequences be replaced with U+FFFD.
 * This means that the tokenizer will not be able to distinguish between
 * different invalid UTF-8 sequences.
 *
 * @param it The iterator to the current position in the UTF-8 stream.
 * @param end The end of the UTF-8 stream.
 * @return The decoded Unicode code-point as a char32_t, or U+FFFD if the
 *         UTF-8 stream is invalid.
 */
template<std::random_access_iterator It>
[[nodiscard]] constexpr char32_t decode_utf8_code_point(It& it, It end) noexcept
    requires (std::same_as<typename std::iterator_traits<It>::value_type, char> or
              std::same_as<typename std::iterator_traits<It>::value_type, char8_t>)
{
    // Should only be called if there are code-units to decode.
    assert(it != end);

    // Get the first byte.
    uint64_t cp = static_cast<uint8_t>(*it++);
    if (cp < 128) [[likely]] {
        return cp;
    }

    // The value in sequence_count is between 0 and 7 inclusive.
    // This is important later on when overlong encoding is checked.
    auto const sequence_count = std::countl_one(static_cast<uint8_t>(cp)) - 1;
    auto const buffer_count = std::distance(it, end);
    if (sequence_count > buffer_count) [[unlikely]] {
        return U'\uFFFD';
    }

    // Strip the leading bits.
    cp &= 0x7f >> sequence_count;

    // Gather the lower 6 bits of each continuation byte.
    // Read `sequence_count + 1` more bytes.
    uint8_t top_bits = 0x00;
    uint8_t i = 0;
    #pragma clang loop unroll(disable)
    do {
        // Invert the top bit so that we can concatonate the value,
        // and keep track if one of the bytes in the sequence is not
        // a continuation byte.
        auto cu = static_cast<uint8_t>(it[i]) ^ 0x80;
        cp <<= 6;
        cp |= cu;
        top_bits |= cu;
    } while (i++ != sequence_count);

    if ((top_bits >> 6) != 0) [[unlikely]] {
        // One of the bytes is not a continuation byte.
        // Find where this non-continuation byte is and set the iterator
        // at this byte.
        while ((static_cast<uint8_t>(*it) >> 6) == 0b10) {
            ++it;
        }
        return U'\uFFFD';

    } else {
        // Before returning the value, make sure the iterator is advanced.
        it += sequence_count;
    }

    // The following check is for:
    // - 0: Invalid, sequence starts with continuation byte.
    // - 1: Valid if `cp` is at least 0x80
    // - 2: Valid if `cp` is at least 0x800
    // - 3: Valid is `cp` is at least 0x1'0000
    // - 4-7: Invalid the maximum sequence is 4 bytes.
    //
    // The 0x38-shift causes min_code_point_value to be 0x8000'0000'0000'0000.
    // The maximum `cp` that can be encoded by an invalid sequence is:
    // 0xff 0xbf 0xbf 0xbf 0xbf 0xbf 0xbf 0xbf 0xbf -> 0x0000'ffff'ffff'ffff
    constexpr auto min_code_point_shifts = uint64_t{0x38'38'38'38'09'04'00'38};

    auto const shift = sequence_count * 8;
    auto min_code_point_value = uint64_t{0x80} << ((min_code_point_shifts >> shift) & 0xff);
    if (cp < min_code_point_value) [[unlikely]] {
        // Invalid sequence count, or overlong encoding.
        return U'\uFFFD';
    }

    if (cp > 0x10ffff) [[unlikely]] {
       // code-point is greater than the maximum valid code-point.
        return U'\uFFFD';
    }

    return static_cast<char32_t>(cp);
}


} // namespace hl
