
#ifndef HL_UTF8_HPP
#define HL_UTF8_HPP

#include <cstdint>
#include <expected>
#include <cstddef>
#include <utility>
#include <array>
#include <bit>
#include <string>

namespace hl {

enum class decode_utf8_error : char32_t {
    /// Invalid UTF-16 surrogate value encoded in UTF-8 stream.
    surrogate = 0xd800,
    /// Found a sole continuation byte.
    continuation_byte = 0xd801,
    /// Found a non-continuation byte in a UTF-8 multi-byte sequence.
    missing_continuation_byte = 0xd802,
    /// UTF-8 multi-byte sequence continues beyond end of buffer.
    buffer_overrun = 0xd803,
    /// Overlong encoding.
    overlong_encoding = 0xd804,
    /// The encoded value was larger than 0x10ffff.
    out_of_range = 0xd805,
};

enum class encode_utf8_error : char32_t {
    /// Invalid UTF-16 surrogate can not be encoded in UTF-8.
    surrogate = 0xd800,
    /// The code-point is larger than 0x10ffff.
    out_of_range = 0xd801,
}; 

/** Decode a single Unicode code-point from a UTF-8 stream.
 *
 * @param [in,out] first The iterator to the current position in the UTF-8 stream.
 * @param last The end of the UTF-8 stream.
 * @return The decoded Unicode code-point.
 * @retval 0xd800 Invalid UTF-16 surrogate value encoded in UTF-8 stream.
 * @retval 0xd801 Found a sole continuation byte.
 * @retval 0xd802 Found a non-continuation byte in a UTF-8 multi-byte sequence.
 * @retval 0xd803 UTF-8 multi-byte sequence continues beyond end of buffer.
 * @retval 0xd804 Overlong encoding.
 * @retval 0xd805 The encoded value was larger than 0x10ffff.
 */
[[nodiscard]] char32_t decode_utf8_code_point(char const*& ptr, char const* end);

[[nodiscard]] std::expected<std::string, encode_utf8_error> encode_utf8_code_point(char32_t cp);

[[nodiscard]] std::string display_utf8_sequence(char const* start, char const* end) noexcept;

} // namespace hl

#endif // HL_UTF8_HPP
