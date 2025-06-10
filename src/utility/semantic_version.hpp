
#pragma once

#include <string_view>
#include <string>
#include <cstddef>
#include <limits>
#include <compare>
#include <format>

namespace hl {

struct semantic_version {
    std::size_t major = 0;
    std::size_t minor = std::numeric_limits<std::size_t>::max();
    std::size_t patch = std::numeric_limits<std::size_t>::max();

    constexpr semantic_version() noexcept = default;
    constexpr semantic_version(semantic_version&&) noexcept = default;
    constexpr semantic_version(const semantic_version&) noexcept = default;
    constexpr semantic_version& operator=(semantic_version&&) noexcept = default;
    constexpr semantic_version& operator=(const semantic_version&) noexcept = default;

    constexpr semantic_version(
        std::size_t major,
        std::size_t minor = std::numeric_limits<std::size_t>::max(),
        std::size_t patch = std::numeric_limits<std::size_t>::max()) noexcept
        : major(major), minor(minor), patch(patch) {}

    semantic_version(std::string_view version);

    [[nodiscard]] constexpr friend bool operator==(semantic_version const& lhs, semantic_version const& rhs) noexcept
    {
        return (lhs <=> rhs) == std::strong_ordering::equal;
    }

    [[nodiscard]] constexpr friend std::strong_ordering operator<=>(semantic_version const& lhs, semantic_version const& rhs) noexcept
    {
        if (lhs.major != rhs.major) {
            return lhs.major <=> rhs.major;
        }

        if (lhs.minor == std::numeric_limits<std::size_t>::max() or rhs.minor == std::numeric_limits<std::size_t>::max()) {
            return std::strong_ordering::equal;
        } else if (lhs.minor != rhs.minor) {
            return lhs.minor <=> rhs.minor;
        }

        if (lhs.patch == std::numeric_limits<std::size_t>::max() or rhs.patch == std::numeric_limits<std::size_t>::max()) {
            return std::strong_ordering::equal;
        } else {
            return lhs.patch <=> rhs.patch;
        }
        return std::strong_ordering::equal;
    }

};

}

template<typename CharT>
struct std::formatter<hl::semantic_version, CharT> : std::formatter<std::basic_string<CharT>, CharT> {
    template <typename FormatContext>
    auto format(hl::semantic_version const& v, FormatContext& ctx) const
    {
        auto minor_str = std::string{"*"};
        if (v.minor != std::numeric_limits<std::size_t>::max()) {
            minor_str = std::format("{}.", v.minor);
        }

        auto patch_str = std::string{"*"};
        if (v.patch != std::numeric_limits<std::size_t>::max()) {
            patch_str = std::format("{}", v.patch);
        }

        return std::format_to(ctx.out(), "{}.{}.{}", v.major, minor_str, patch_str);
    }
};

