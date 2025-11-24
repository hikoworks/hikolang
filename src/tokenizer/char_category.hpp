
#pragma once

#include <cassert>
#include <utility>

/** @file char_category.hpp
 * 
 * This file defines various character categories and utility functions
 * to check if a code-point belongs to a specific category.
 */

namespace hk {

char32_t advance_cp(char const*& p) noexcept
{
    auto cu = static_cast<uint8_t>(*p++);
    if (cu & 0x80 == 0) {
        return static_cast<char32_t>(cu);
    }

    auto n = std::countl_one(cu);
    cu <<= n;
    cu >>= n;
    auto cp = static_cast<char32_t>(cu);

    while (--n) {
        cp <<= 6;
        cp |= static_cast<char32_t>(*p++) & 0x3f;
    }
    return cp;
}

void advance_cp(char const*& p, size_t n)
{
    assert(n != 0);
    do {
        assert(*p != '\0');
        do {
            ++p;
        } while ((*p & 0xc0) == 0x80);
    } while (--n);
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
 * @param c A pointer to the characters to read (+2 lookahead)
 * @return Number of code-units to skip. or 0 if not a vertical-space.
 */
[[nodiscard]] constexpr bool is_vertical_space(char const* c) noexcept
{
    if (c[0] == '\n' or c[0] == '\v' or c[0] == '\f') {
        return 1;
    } else if (c[0] == '\r' and c[1] != '\n') {
        return 1; // lone CR on old style MacOS
    } else if (c[0] == 0xc2 and c[1] == 0x85) {
        return 2; // U+0085
    } else if (c[0] == 0xe2 and c[1] == 0x80 and (c[2] == 0xa8 or c[2] == 0xa9)) {
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
[[nodiscard]] constexpr bool is_horizontal_space(char32_t cp, char32_t cp2) noexcept
{
    if (cp == '\r') {
        if (cp2 == '\n') {
            // CR is a horizontal space when it is followed by LF.
            return true;
        } else {
            // A lone CR is not considered a vertical space.
            return false;
        }
    }

    return cp == '\t' or cp == ' ' or cp == U'\u00A0' or cp == U'\u1680' or (cp >= U'\u2000' and cp <= U'\u200A') or
        cp == U'\u202F' or cp == U'\u205F' or cp == U'\u3000';
}

/** Is a code-point a bracket.
 * 
 * A bracket includes:
 * - `{` (Left Brace)
 * - `[` (Left Square Bracket)
 * - `(` (Left Parenthesis)
 * - `}` (Right Brace)
 * - `]` (Right Square Bracket)
 * - `)` (Right Parenthesis)
 * 
 */
[[nodiscard]] constexpr bool is_bracket(char32_t cp) noexcept
{
    return cp == '{' or cp == '[' or cp == '(' or cp == '}' or cp == ']' or cp == ')';
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

/** Is a code-point a separator.
 * 
 * A separator includes:
 * - `,` (Comma)
 * - `;` (Semicolon)
 * 
 */
[[nodiscard]] constexpr bool is_separator(char32_t cp) noexcept
{
    return cp == ',' or cp == ';';
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

/** Is a code-point ignoreable.
 * 
 * An ignoreable code-point is one that can be safely ignored in the context of parsing.
 * 
 * Currently, the only ignoreable code-point is the Byte Order Mark (BOM) `U+FEFF`.
 * 
 * @param cp The code-point to check.
 * @retval true if the code-point is ignoreable.
 */
[[nodiscard]] constexpr bool is_ignoreable(char32_t cp) noexcept
{
    return cp == U'\uFEFF';
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
