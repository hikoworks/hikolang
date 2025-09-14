
#pragma once

#include <utility>

namespace hk {

enum class repository_flags {
    /** The repository was freshly cloned, not need for a fetch.
     */
    fresh_clone = 0x1,

    /** Fetch, even when the rev points to a tag or commit.
     */
    force_fetch = 0x2,

    /** Force the repository to be cleaned, even if no checkout is performed.
     */
    force_clean = 0x4,

    /** Force all files to scan, even if modifcation date hasn't changed.
     */
    force_scan = 0x8,
};

[[nodiscard]] constexpr repository_flags operator|(repository_flags lhs, repository_flags rhs) noexcept
{
    return static_cast<repository_flags>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

[[nodiscard]] constexpr repository_flags operator&(repository_flags lhs, repository_flags rhs) noexcept
{
    return static_cast<repository_flags>(std::to_underlying(lhs) & std::to_underlying(rhs));
}

[[nodiscard]] constexpr repository_flags operator~(repository_flags rhs) noexcept
{
    return static_cast<repository_flags>(~std::to_underlying(rhs));
}

constexpr repository_flags& operator|=(repository_flags& lhs, repository_flags rhs) noexcept
{
    return lhs = lhs | rhs;
}

constexpr repository_flags& operator&=(repository_flags& lhs, repository_flags rhs) noexcept
{
    return lhs = lhs & rhs;
}

[[nodiscard]] constexpr bool to_bool(repository_flags rhs) noexcept
{
    return static_cast<bool>(std::to_underlying(rhs));
}

}