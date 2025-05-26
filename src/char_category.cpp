
#include "char_category.hpp"
#include <unicode/uchar.h>

namespace hl {

[[nodiscard]] bool is_identifier_start(char32_t cp) noexcept
{
    return u_hasBinaryProperty(cp, UCHAR_ID_START) or cp == '_';
}

[[nodiscard]] bool is_identifier_continue(char32_t cp) noexcept
{
    return u_hasBinaryProperty(cp, UCHAR_ID_CONTINUE) or cp == '_';
}

[[nodiscard]] bool is_pattern_syntax(char32_t cp) noexcept
{
    return u_hasBinaryProperty(cp, UCHAR_PATTERN_SYNTAX);
}

} // namespace hl