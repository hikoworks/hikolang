
#pragma once

#include <cassert>
#include <utility>
#include <bit>
#include <cstdint>
#include <cstring>
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
[[nodiscard]] std::pair<char32_t, uint8_t> get_cp(char const* const p) noexcept;

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
    auto p0 = static_cast<uint8_t>(p[0]);
    auto p1 = static_cast<uint8_t>(p[1]);
    auto p2 = static_cast<uint8_t>(p[2]);

    if (p0 == '\n' or p0 == '\v' or p0 == '\f') {
        return 1;
    } else if (p0 == '\r' and p1 != '\n') {
        return 1; // lone CR on old style MacOS
    } else if (p0 == 0xc2 and p1 == 0x85) {
        return 2; // U+0085
    } else if (p0 == 0xe2 and p1 == 0x80 and (p2 == 0xa8 or p2 == 0xa9)) {
        return 3; // U+2028 or U+2029
    } else {
        return 0;
    }
}

template<typename T, T... Values>
[[nodiscard]] constexpr bool match(T value) noexcept
{
    return ((Values == value) or ...);
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
