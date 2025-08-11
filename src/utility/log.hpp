
#pragma once

#include <utility>
#include <print>

namespace hk {

enum class log_mask {
    /** Display progress for downloading.
     */
    progress,

    /** Use ANSI codes to display progress prettier.
     */
    color,

    /** Display debug information.
     */
    debug,

    /** Display verbose information when using resources outside the compiler.  
     */
    verbose,

    /** Display warnings and errors for the compiler.
     * 
     * Normally enabled, but disabled in case of a language server.
     */
    language,
};

[[nodiscard]] constexpr log_mask operator|(log_mask lhs, log_mask rhs) noexcept
{
    return static_cast<log_mask>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

[[nodiscard]] constexpr log_mask operator&(log_mask lhs, log_mask rhs) noexcept
{
    return static_cast<log_mask>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

[[nodiscard]] constexpr bool to_bool(log_mask rhs) noexcept
{
    return static_cast<bool>(std::to_underlying(rhs));
}

inline log_mask global_log_mask = log_mask::progress | log_mask::color | log_mask::language;

template<log_mask Mask, typename... Args>
void log(std::format_string<Args...> fmt, Args &&... args)
{
    if (to_bool(Mask & global_log_mask)) {
        std::print(fmt, std::forward<Args>(args)...);
    }
}

}