
#ifndef HL_CHAR_CATEGORY_HPP
#define HL_CHAR_CATEGORY_HPP

namespace hl {

[[nodiscard]] constexpr bool is_vertical_space(char32_t cp) noexcept
{
    return cp == '\n' or cp == '\v' or cp == '\f' or cp == '\r' or cp == U'\u0085' or cp == U'\u2028' or cp == U'\u2029';
}

[[nodiscard]] constexpr bool is_horizontal_space(char32_t cp) noexcept
{
    return cp == '\t' or cp == ' ' or cp == U'\u00A0' or cp == U'\u1680' or (cp >= U'\u2000' and cp <= U'\u200A') or
        cp == U'\u202F' or cp == U'\u205F' or cp == U'\u3000';
}

[[nodiscard]] constexpr bool is_bracket(char32_t cp) noexcept
{
    return cp == '{' or cp == '[' or cp == '(' or cp == '}' or cp == ']' or cp == ')';
}

[[nodiscard]] constexpr bool is_separator(char32_t cp) noexcept
{
    return cp == ',' or cp == ';';
}

[[nodiscard]] constexpr bool is_digit(char32_t cp) noexcept
{
    return cp >= '0' and cp <= '9';
}

[[nodiscard]] constexpr bool is_ignoreable(char32_t cp) noexcept
{
    return cp == U'\uFEFF';
}

[[nodiscard]] bool is_identifier_start(char32_t cp) noexcept;

[[nodiscard]] bool is_identifier_continue(char32_t cp) noexcept;

[[nodiscard]] bool is_pattern_syntax(char32_t cp) noexcept;

} // namespace hl

#endif // HL_CHAR_CATEGORY_HPP
