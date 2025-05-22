
#include <uci.h>

namespace hl {

[[nodiscard]] constexpr bool is_vertical_space(char32_t cp) noexcept
{
    return cp == '\n' or cp == '\v' or cp == '\f' or cp == '\r' or cp == '\u0085' or cp == '\u2028' or cp == '\u2029';
}

[[nodiscard]] constexpr bool is_horizontal_space(char32_t cp) noexcept
{
    return cp == '\t' or cp == ' ' or cp == '\u00A0' or cp == '\u1680' or (cp >= '\u2000' and cp <= '\u200A') or cp == '\u202F' or
        cp == '\u205F' or cp == '\u3000';
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

[[nodiscard]] constexpr bool is_identifier_start(char32_t cp) noexcept
{
    return uci_is_id_start(cp) or cp == '_';
}

[[nodiscard]] constexpr bool is_identifier_continue(char32_t cp) noexcept
{
    return uci_is_id_continue(cp) or cp == '_';
}

[[nodiscard]] constexpr bool is_pattern_syntax(char32_t cp) noexcept
{
    return uci_is_pattern_syntax(cp);
}

}
