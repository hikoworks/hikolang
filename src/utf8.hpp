
#include <iterator>
#include <cstdint>
#include <expected>
#include <cstddef>
#include <cassert>
#include <utility>
#include <array>
#include <string>

namespace hl {

[[nodiscard]] constexpr char32_t decode_utf8_code_point(uint8_t const*& ptr, uint8_t const* ptr_end) noexcept
{
    constexpr auto replacement_character = 0xfffd;

    // Should only be called if there are code-units to decode.
    assert(it != end);

    // Get the first byte.
    uint64_t cp = *ptr++;
    if (cp < 128) [[likely]] {
        return cp;
    }

    auto count = std::countl_one(static_cast<uint8_t>(cp)) - 1;
    auto const seq_ptr_end = ptr + count;
    if (seq_ptr_end > ptr_end) [[unlikely]] {
        return replacement_character;
    }

    // Strip the leading bits.
    cp &= 0x7f >> count;

    uint8_t top_bits = 0x00;
    #pragma clang loop unroll(disable)
    for (; ptr != seq_ptr_end; ++ptr) {
        // Invert the top bit so that we can concatonate the value.
        auto cu = static_cast<uint8_t>(*ptr) ^ 0x80;
        cp <<= 6;
        cp |= cu;
        top_bits |= cu;
    }

    if (top_bits != 0) [[unlikely]] {
        return replacement_character;
    }

    if (cp > 0x10ffff) [[unlikely]] {
       // code-point is greater than the maximum valid code-point.
       return replacement_character;
    }

    // Using 8 bit per shift value, reduces an (AND r,15) instruction.
    count *= 8;
    constexpr auto min_code_point_shifts = uint64_t{0x38'38'38'38'09'04'00'38};
    auto min_code_point_value = uint64_t{0x80} << ((min_code_point_shifts >> count) & 0xff);
    if (cp < min_code_point_value) [[unlikely]] {
        // Invalid sequence count, or overlong encoding.
        return replacement_character;
    }

    return static_cast<char32_t>(cp);
}

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
    auto ptr = reinterpret_cast<uint8_t const *>(&it);
    auto const end_ptr = reinterpret_cast<uint8_t const *>(&end);

    auto const ptr_prev = ptr;
    auto const r = decode_utf8_code_point(ptr, end_ptr);
    it += std::distance(ptr_prev, ptr);
    return r;
}

} // namespace hl

[[nodiscard]] char32_t test(std::string tmp, std::string::const_iterator &out) noexcept
{
    auto it = tmp.cbegin();
    auto const end = tmp.cend();
    auto const v = hl::decode_utf8_code_point(it, end);
    out = it;
    return v;
}