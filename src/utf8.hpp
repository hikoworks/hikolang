
#include <cstdint>
#include <expected>
#include <cstddef>
#include <utility>
#include <array>
#include <bit>

namespace hl {

enum decode_utf8_error : uint32_t {
    /// Found a sole continuation byte.
    continuation_byte = 0x11'0000,
    /// Found a non-continuation byte in a UTF-8 multi-byte sequence.
    missing_continuation_byte = 0x11'0001,
    /// UTF-8 multi-byte sequence continues beyond end of buffer.
    buffer_overrun = 0x11'0002,
    /// Overlong encoding.
    overlong_encoding = 0x11'0003,
    /// The encoded value was larger than 0x10ffff.
    out_of_range = 0x11'0004,
    /// Invalid UTF-16 surrogate value encoded in UTF-8 stream.
    surrogate = 0x11'0005,
};

/** Decode a single Unicode code-point from a UTF-8 stream.
 *
 * @param [in,out] first The iterator to the current position in the UTF-8 stream.
 * @param last The end of the UTF-8 stream.
 * @return The decoded Unicode code-point.
 * @retval 0x110000 Found a sole continuation byte.
 * @retval 0x110001 Found a non-continuation byte in a UTF-8 multi-byte sequence.
 * @retval 0x110002 UTF-8 multi-byte sequence continues beyond end of buffer.
 * @retval 0x110003 Overlong encoding.
 * @retval 0x110004 The encoded value was larger than 0x10ffff.
 * @retval 0x110005 Invalid UTF-16 surrogate value encoded in UTF-8 stream.
 */
[[nodiscard]] constexpr uint32_t decode_utf8_code_point(char const*& ptr, char const* end);


} // namespace hl
