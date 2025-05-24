#include <iterator>
#include <cstdint>
#include <expected>
#include <cstddef>
#include <cassert>
#include <utility>
#include <array>
#include <string>
#include <bit>

namespace hl {

enum decode_utf8_error : uint32_t {
    /// Found a sole continuation byte.
    continuation_byte = 0x11'0000,
    /// Found a non-continuation byte in a UTF-8 multi-byte sequence.
    missing_continuation_byte = 0x11'0001,
    /// UTF-8 multi-byte sequence continues beyond end of buffer.
    buffer_overrun = 0x11'0002,
    /// Overlong encoding
    overlong_encoding = 0x11'0003,
    /// The encoded value was larger than 0x10ffff.
    out_of_range = 0x11'0004,
    /// Invalid UTF-16 surrogate value encoded in UTF-8 stream.
    surrogate = 0x11'0005,
};

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
 * @retval 0x110000 Continuation-byte as first byte.
 */
template<std::random_access_iterator It>
   [[nodiscard]] constexpr uint32_t decode_utf8_code_point(It& it, It const end) noexcept
    requires (std::same_as<typename std::iterator_traits<It>::value_type, char> or
              std::same_as<typename std::iterator_traits<It>::value_type, char8_t>)
{
    // Should only be called if there are code-units to decode.
    assert(it != end);

    // Get the first byte.
    auto first_cu = static_cast<uint8_t>(*it++);
    if (first_cu < 128) [[likely]] {
        return first_cu;
    }
    first_cu <<= 1;

    if (first_cu < 128) [[unlikely]] {
        return std::to_underlying(decode_utf8_error::continuation_byte);
    }

    auto const seq_it = it;
    uint64_t cp = 0;
    #pragma clang loop unroll(disable)
    do {
        first_cu <<= 1;

        if (it == end) [[unlikely]] {
            return std::to_underlying(decode_utf8_error::buffer_overrun);
        }

        uint8_t cu = static_cast<uint8_t>(*it++) ^ 0x80;
        cp <<= 6;
        cp |= cu;

        if (cu >> 6 != 0) [[unlikely]] {
            return std::to_underlying(decode_utf8_error::missing_continuation_byte);
        }
    } while (first_cu >= 128);

    auto const i = std::distance(seq_it, it) - 1;

    // Prefix the code-point with the first code-unit.
    // The first code-unit is already stripped from leading bits, but must be shifted back.
    cp |= first_cu << (i * 5 + 4);

    // The loop can be executed maximum 7 times, before first_cu is guaranteed zero.
    //            v
    // 0:       110xxxxx : 2 byte sequence (U+80 - U+7FF)
    // 1:      1110xxxx  : 3 byte sequence (U+0800 - U+FFFF)
    // 2:     11110xxx   : 4 byte sequence (U+010000 - U+10FFFF)
    // 3:    111110xx    : invalid 5 byte sequence
    // 4:   1111110x     : invalid 6 byte sequence
    // 5:  11111110      : invalid 7 byte sequence
    // 6: 11111111       : invalid 8 byte sequence
    constexpr auto minimum_cp_shift = uint64_t{0x00'38'38'38'38'09'04'00};
    auto const minimum_cp = uint64_t{0x80} << static_cast<uint8_t>(minimum_cp_shift >> (i * 8));
    if (cp < minimum_cp) [[unlikely]] {
        return std::to_underlying(decode_utf8_error::overlong_encoding);
    }

    if (cp > 0x10ffff) [[unlikely]] {
        return std::to_underlying(decode_utf8_error::out_of_range);
    }

    if (cp >= 0xd800 and cp <= 0xdfff) [[unlikely]] {
        return std::to_underlying(decode_utf8_error::surrogate);
    }

    return cp;
}


} // namespace hl
