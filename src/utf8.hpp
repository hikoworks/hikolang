
#include <iterator>
#include <cstdint>
#include <expected>
#include <cstddef>
#include <cassert>
#include <utility>
#include <array>

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
    assert(it != end);

    auto tmp_it = it;

    auto cu = static_cast<uint8_t>(*tmp_it++);
    if (cu < 128) {
        it = tmp_it;
        return cu;
    }

    auto count = std::countl_one(cu);
    if (std::distance(it, end) < count) {
        return U'\uFFFD';
    }
    auto const tmp_end = it + count;

    // Strip the leading bits.
    cu <<= count;
    cu >>= count;

    auto cp = static_cast<uint64_t>(cu);

    auto mask = uint8_t{};
    #pragma clang loop unroll(disable)
    do {
        assert(tmp_it != tmp_end);
        cu = static_cast<uint8_t>(*tmp_it++);
        cu ^= 0x80;
        mask |= cu;
        cp <<= 6;
        cp |= cu;
    } while (tmp_it != tmp_end);

    constexpr auto replacement_character = 0xfffd;

    mask >>= 6;
    if (mask != 0x00) {
        // Top bit of continuation character should all be 1.
        cp = replacement_character;
    }

    // The shift values to turn 0x80 into a minimum code-point value:
    // 0 -> 0
    // 1 -> 0x80
    // 2 -> 0x0800
    // 3 -> 0x01'0000
    // 4-7 -> 0
    constexpr auto min_code_point_value_shifts = uint64_t{0x1f'1f'1f'1f'09'04'00'1f};

    assert(count != 7 and count <= 8);
    auto min_code_point_value_shift = static_cast<uint8_t>(min_code_point_value_shifts >> ((count - 1) * 8));

    auto min_code_point_value = uint32_t{0x80} << min_code_point_value_shift;

    if (cp < min_code_point_value) {
        // Invalid sequence count, or overlong encoding.
        cp = replacement_character;
    }
    
    if (cp > 0x10FFFF) {
        // code-point is greater than the maximum valid code-point.
        cp = replacement_character;
    }

    if (cp >= 0xD800 and cp <= 0xDFFF) {
        // Surrogate code-points are not valid in UTF-8.
        cp = replacement_character;
    }

    // Check for invalid code-point.
    it = tmp_end;
    return static_cast<char32_t>(cp);
}

} // namespace hl

