
#include "char_category.hpp"

extern "C" {
#include <unicode/uchar.h>
}

namespace hk {

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