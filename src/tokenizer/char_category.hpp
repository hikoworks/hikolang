
#pragma once

#include <cassert>
#include <utility>
#include <bit>
#include "utility/fixed_string.hpp"

/** @file char_category.hpp
 * 
 * This file defines various character categories and utility functions
 * to check if a code-point belongs to a specific category.
 */

namespace hk {

/** Get a code-point.
 *
 * The following errors are detected by this function:
 *  - Unexpected continuation bytes
 *  - Invalid byte (originally mapped to sequences larger than 4 bytes)
 *  - Missing continuation bytes: the length will skip over the valid part
 *    of the sequence.
 * 
 * Other errors with invalid Unicode code-points and overlong encoding should be
 * checked outside of this function.
 *
 * @param p Pointer to the next code-point in an UTF-8 stream.This function will
 *       read 3 bytes beyond the pointer, make sure the buffer has at least 3
 *       extra bytes at the end.
 * @return code-point, number-of-code-units. On error U+FFFD is returned.
 */
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
        bytes >>= 8;
        auto const cu = static_cast<uint8_t>(bytes);
        cp <<= 6;
        cp |= cu;
    } while (--n);

    return {cp, length + 1};
}

/** Advance the UTF-8 stream pointer by a number of code-points.
 *
 * This uses get_cp() to properly advance over code-points including over
 * errors.
 *
 * @param p A pointer to the start of a code-point in a UTF-8 stream
 * @param n Number of code-points to skip.
 * @return Number of code-units to skip.
 */
[[nodiscard]] size_t advance_cp(char const* p, size_t n) noexcept
{
    auto r = 0uz;

    assert(n != 0);
    do {
        assert(*p != '\0');

        auto const [_, count] = get_cp(p);
        r += count;
    } while (--n);

    return r;
}

/** Is a code-point a vertical space.
 * 
 * A vertical space includes:
 *  - `\n` (LF, Line Feed)
 *  - `\r` (CR, Carriage Return) but only if NOT followed by `\n`
 *  - `\v` (Vertical Tab)
 *  - `\f` (Form Feed)
 *  - `\u0085` (Next Line)
 *  - `\u2028` (Line Separator)
 *  - `\u2029` (Paragraph Separator)
 * 
 * @param p A pointer to the characters to read (+2 lookahead)
 * @return Number of code-units to skip. or 0 if not a vertical-space.
 */
[[nodiscard]] constexpr size_t is_vertical_space(char const* p) noexcept
{
    if (p[0] == '\n' or p[0] == '\v' or p[0] == '\f') {
        return 1;
    } else if (p[0] == '\r' and p[1] != '\n') {
        return 1; // lone CR on old style MacOS
    } else if (p[0] == 0xc2 and p[1] == 0x85) {
        return 2; // U+0085
    } else if (p[0] == 0xe2 and p[1] == 0x80 and (p[2] == 0xa8 or p[2] == 0xa9)) {
        return 3; // U+2028 or U+2029
    } else {
        return 0;
    }
}

/** Is a code-point a horizontal space.
 * 
 * A horizontal space includes:
 *  - `\t` (Tab)
 *  - ` ` (Space)
 *  - `\r` (Carriage Return) but only if followed by `\n`
 *  - `\u00A0` (Non-breaking Space)
 *  - `\u1680` (Ogham Space Mark)
 *  - `\u2000` to `\u200A` (Various Space Characters)
 *  - `\u202F` (Narrow No-Break Space)
 *  - `\u205F` (Medium Mathematical Space)
 *  - `\u3000` (Ideographic Space)
 */
[[nodiscard]] constexpr size_t is_horizontal_space(char const* p) noexcept
{
    if (p[0] == '\t' or p[0] == ' ') {
        return 1;
    
    } else if (p[0] == '\r' and p[1] != '\n') {
        return 1;

    } else if (p[0] == 0xc2 and p[1] == 0xa0) {
        return 2; // U+00A0

    } else if (p[0] == 0xe1 and p[1] == 0x9a and p[2] == 0x80) {
        return 3; // U+1680

    } else if (p[0] == 0xe2) {
        if (p[1] == 0x80 and ((p[2] > 0x80 and p[2] < 0x8a) or p[2] == 0xaf)) {
            return 3; // U+2000..U+200A, U+202F
        } else if (p[1] == 0x81 or p[2] == 0x9f) {
            return 3; // U+205F
        } else {
            return 0;
        }
    
    } else if (p[0] == 0xe3 and p[1] == 0x80 and p[2] == 0x80) {
        return 3; // U+3000

    } else {
        return 0;
    }
}

template<fixed_string Characters>
[[nodiscard]] constexpr char match(char c) noexcept
{
    for (auto i = 0uz; i != Characters.size(); ++i) {
        if (Characters[i] == c) {
            return c;
        }
    }
    return '\0';
}

/** Mirror a bracket code-point.
 * 
 * This function returns the corresponding mirror bracket for a given bracket code-point.
 * 
 * - `{` becomes `}`
 * - `}` becomes `{`
 * - `[` becomes `]`
 * - `]` becomes `[`
 * - `(` becomes `)`
 * - `)` becomes `(`
 * 
 * @note It is UNDEFINED BEHAVIOR to call this function with a code-point that is not a bracket.
 * @param cp The code-point to mirror.
 * @retval The mirrored code-point.
 */
[[nodiscard]] constexpr char32_t mirror_bracket(char32_t cp)
{
    // clang-format off
    switch (cp) {
    case '{': return '}';
    case '}': return '{';
    case '[': return ']';
    case ']': return '[';
    case '(': return ')';
    case ')': return '(';
    default:
        std::unreachable();
    }
}


/** Is a code-point a digit.
 * 
 * Digits are defined based on the radix:
 *  - Radix 2: `0`, `1`
 *  - Radix 8: `0` to `7`
 *  - Radix 10: `0` to `9`
 *  - Radix 16: `0` to `9`, `a` to `f`, `A` to `F`
 * 
 * @param cp The code-point to check.
 * @param radix The radix to check against, defaults to 10.
 * @retval true if the code-point is a digit in the given radix.
 */
[[nodiscard]] constexpr bool is_digit(char32_t cp, unsigned int radix = 10) noexcept
{
    if (radix == 2) {
        return cp == '0' or cp == '1';
    } else if (radix == 8) {
        return cp >= '0' and cp <= '7';
    } else if (radix == 10) {
        return cp >= '0' and cp <= '9';
    } else if (radix == 16) {
        return (cp >= '0' and cp <= '9') or (cp >= 'a' and cp <= 'f') or (cp >= 'A' and cp <= 'F');
    }
    return false;
}

/** Is a code-point a superscript-digit.
 * 
 * Digits are defined based on the radix:
 *  - Radix 10: `⁰` to `⁹`
 * 
 * @param cp The code-point to check.
 * @param radix The radix to check against, defaults to 10.
 * @retval true if the code-point is a digit in the given radix.
 */
[[nodiscard]] constexpr bool is_superscript_digit(char32_t cp) noexcept
{
    return
        cp == U'⁰' or
        cp == U'¹' or
        cp == U'²' or
        cp == U'³' or
        cp == U'⁴' or
        cp == U'⁵' or
        cp == U'⁶' or
        cp == U'⁷' or
        cp == U'⁸' or
        cp == U'⁹';
}

/** Is a code-point an exponent prefix.
 * 
 * An exponent prefix is used in floating-point numbers to indicate the exponent part.
 * 
 * For radix 16, the prefix is `p` or `P`.
 * For other radices, the prefix is `e` or `E`.
 * 
 * @param cp The code-point to check.
 * @param radix The radix to check against.
 * @retval true if the code-point is an exponent prefix in the given radix.
 */
[[nodiscard]] constexpr bool is_exponent_prefix(char32_t cp, int radix) noexcept
{
    if (radix == 16) {
        return cp == 'p' or cp == 'P';
    } else {
        return cp == 'e' or cp == 'E';
    }
}

/** Check if a code-point is a valid identifier start character.
 * 
 * An identifier start character is one that can be used to start an identifier.
 * 
 * This includes:
 * - Letters (a-z, A-Z)
 * - Underscore (_)
 * - And many other characters defined by the Unicode standard as valid identifier start characters.
 * 
 * @param cp The code-point to check.
 * @retval true if the code-point is a valid identifier start character.
 */
[[nodiscard]] bool is_identifier_start(char32_t cp) noexcept;

/** Check if a code-point is a valid identifier continuation character.
 * 
 * An identifier continuation character is one that can be used to continue an identifier.
 * 
 * This includes:
 * - Letters (a-z, A-Z)
 * - Digits (0-9)
 * - Underscore (_)
 * - And many other characters defined by the Unicode standard as valid identifier continuation characters.
 * 
 * @param cp The code-point to check.
 * @retval true if the code-point is a valid identifier continuation character.
 */
[[nodiscard]] bool is_identifier_continue(char32_t cp) noexcept;

/** Check if a code-point is a pattern syntax character.
 * 
 * A pattern syntax character is one that is used in regular expressions and other patterns.
 * 
 * This includes:
 * - `*` (Asterisk)
 * - `+` (Plus)
 * - `?` (Question Mark)
 * - `|` (Pipe)
 * - And many other characters defined by the Unicode standard as pattern syntax characters.
 * 
 * @param cp The code-point to check.
 * @retval true if the code-point is a pattern syntax character.
 */
[[nodiscard]] bool is_pattern_syntax(char32_t cp) noexcept;

} // namespace hk
