
#include "char_category.hpp"

extern "C" {
#include <unicode/uchar.h>
}

namespace hk {

[[nodiscard]] std::pair<char32_t, uint8_t> get_cp(char const* const p) noexcept
{
    auto bytes = uint32_t{};
    std::memcpy(&bytes, p, sizeof(bytes));

    if constexpr (std::endian::native == std::endian::big) {
        bytes = std::byteswap(bytes);
    }

    auto const ascii = static_cast<int8_t>(static_cast<uint8_t>(bytes));
    if (ascii >= 0) [[likely]] {
        return {ascii, 1};
    }

    auto const length_table = uint32_t{0b00'11'10'10'01'01'01'01'00'00'00'00'00'00'00'00};
    auto const length_index = (bytes >> 2) & 0b11110;
    auto const length = static_cast<uint8_t>((length_table >> length_index) & 0b11);
    if (length == 0) [[unlikely]] {
        // If length is zero, the first code-unit was a continuation byte, or it
        // has an illegal value.
        return {0xfffd, 1};
    }

    // Flip the top bit of potential continuation bytes.
    // This makes it easy to test and append the continuation bytes;
    // Both top bits will be zero if they are continuation bytes.
    bytes ^= 0x80808000;

    // Clear the unused bytes on the left.
    auto const length8 = length * 8;
    bytes <<= length8;
    bytes >>= length8;

    // Check the top two bits if they match continuation bytes, both must be zero.
    if (bytes & 0xc0c0c000) [[unlikely]] {
        if (bytes & 0x0000c000) {
            return {0xfffd, 1};
        } else if (bytes & 0x00c00000) {
            return {0xfffd, 2};
        } else {
            return {0xfffd, 3};
        }
    }

    auto n = length;
    auto cp = bytes & (0x3f >> length);
    do {
        cp <<= 6;
        cp |= static_cast<uint8_t>(bytes >>= 8);
    } while (--n);

    return {cp, length + 1};
}

[[nodiscard]] bool is_identifier_start(char32_t cp) noexcept
{
    return ::u_hasBinaryProperty(static_cast<UChar32>(cp), UCHAR_ID_START) or cp == '_'  or cp == U'°';
}

[[nodiscard]] bool is_identifier_continue(char32_t cp) noexcept
{
    return ::u_hasBinaryProperty(static_cast<UChar32>(cp), UCHAR_ID_CONTINUE) or cp == '_' or cp == U'°';
}

[[nodiscard]] bool is_pattern_syntax(char32_t cp) noexcept
{
    return ::u_hasBinaryProperty(static_cast<UChar32>(cp), UCHAR_PATTERN_SYNTAX) and cp != '_' and cp != U'°';
}

} // namespace hk