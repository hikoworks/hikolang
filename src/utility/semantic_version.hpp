
#pragma once

#include <string_view>
#include <string>
#include <cstddef>
#include <limits>
#include <compare>
#include <format>

namespace hk {

/** A semantic versioning structure.
 * 
 * This structure represents a semantic version in the format "major.minor.patch".
 * The minor and patch versions can be set to `std::numeric_limits<std::size_t>::max()`
 * to indicate that they are wild cards.
 */
struct semantic_version {
    std::size_t major = std::numeric_limits<std::size_t>::max();
    std::size_t minor = std::numeric_limits<std::size_t>::max();
    std::size_t patch = std::numeric_limits<std::size_t>::max();

    constexpr semantic_version(semantic_version&&) noexcept = default;
    constexpr semantic_version(const semantic_version&) noexcept = default;
    constexpr semantic_version& operator=(semantic_version&&) noexcept = default;
    constexpr semantic_version& operator=(const semantic_version&) noexcept = default;

    /** Create a semantic version with the given major, minor and patch versions.
     * 
     * The minor and patch versions can be set to `std::numeric_limits<std::size_t>::max()`
     * to indicate that they are wild cards.
     * 
     * @param major The major version.
     * @param minor The minor version, defaults to `std::numeric_limits<std::size_t>::max()`.
     * @param patch The patch version, defaults to `std::numeric_limits<std::size_t>::max()`.
     */
    constexpr semantic_version(
        std::size_t major = std::numeric_limits<std::size_t>::max(),
        std::size_t minor = std::numeric_limits<std::size_t>::max(),
        std::size_t patch = std::numeric_limits<std::size_t>::max()) noexcept
        : major(major), minor(minor), patch(patch) {}

    /** Create a version number that is the highest possible.
     */
    [[nodiscard]] constexpr static semantic_version make_max() noexcept
    {
        auto r = semantic_version{};
        r.major = std::numeric_limits<std::size_t>::max() - 1;
        r.minor = std::numeric_limits<std::size_t>::max() - 1;
        r.patch = std::numeric_limits<std::size_t>::max() - 1;
        return r;
    } 

    /** Create a semantic version from a string.
     * 
     * The string should be in the format "major.minor.patch", where minor and patch can be
     * replaced with `*` to indicate that they are wild cards.
     * 
     * @param version The version string to parse.
     * @throws std::invalid_argument if the version string is not in the correct format.
     */
    semantic_version(std::string_view version);

    /** Check if the minor version is a wild card.
     * 
     * A wild card is represented by `std::numeric_limits<std::size_t>::max()`.
     * 
     * @return true if the minor version is a wild card, false otherwise.
     */
    [[nodiscard]] constexpr bool minor_is_wildcard() const noexcept
    {
        return minor == std::numeric_limits<std::size_t>::max();
    }

    /** Check if the patch version is a wild card.
     * 
     * A wild card is represented by `std::numeric_limits<std::size_t>::max()`.
     * 
     * @return true if the patch version is a wild card, false otherwise.
     */
    [[nodiscard]] constexpr bool patch_is_wildcard() const noexcept
    {
        return patch == std::numeric_limits<std::size_t>::max();
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return major == std::numeric_limits<std::size_t>::max();
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return not empty();
    } 

    /** Check if this semantic version is equal to another semantic version.
     * 
     * Two semantic versions are equal if their major, minor and patch versions are equal,
     * or if the minor or patch version is a wild card.
     * 
     * @param other The other semantic version to compare with.
     * @return true if the two semantic versions are equal, false otherwise.
     */
    [[nodiscard]] constexpr friend bool operator==(semantic_version const& lhs, semantic_version const& rhs) noexcept
    {
        return (lhs <=> rhs) == std::strong_ordering::equal;
    }

    /** Compare this semantic version with another semantic version.
     * 
     * The comparison is done based on the major, minor and patch versions.
     * If the minor or patch version is a wild card, it is considered equal to any other wild card.
     * 
     * @param other The other semantic version to compare with.
     * @return A strong ordering indicating the result of the comparison.
     */
    [[nodiscard]] constexpr friend std::strong_ordering operator<=>(semantic_version const& lhs, semantic_version const& rhs) noexcept
    {
        if (lhs.major != rhs.major) {
            if (lhs.major == std::numeric_limits<size_t>::max()) {
                return std::strong_ordering::less;
            } else if (rhs.major == std::numeric_limits<size_t>::max()) {
                return std::strong_ordering::greater;
            } else {
                return lhs.major <=> rhs.major;
            }
        }

        if (lhs.minor_is_wildcard() or rhs.minor_is_wildcard()) {
            return std::strong_ordering::equal;
        } else if (lhs.minor != rhs.minor) {
            return lhs.minor <=> rhs.minor;
        }

        if (lhs.patch_is_wildcard() or rhs.patch_is_wildcard()) {
            return std::strong_ordering::equal;
        } else {
            return lhs.patch <=> rhs.patch;
        }

        return std::strong_ordering::equal;
    }

};

}

template<typename CharT>
struct std::formatter<hk::semantic_version, CharT> : std::formatter<std::basic_string<CharT>, CharT> {
    template <typename FormatContext>
    auto format(hk::semantic_version const& v, FormatContext& ctx) const
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

