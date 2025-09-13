
#pragma once

#include <string>
#include <compare>

namespace hk {

enum class repository_type {
    none,
    git,
    zip
};

class repository_url {
public:
    constexpr repository_url() noexcept = default;
    constexpr repository_url(repository_url const&) noexcept = default;
    constexpr repository_url(repository_url&&) noexcept = default;
    constexpr repository_url& operator=(repository_url const&) noexcept = default;
    constexpr repository_url& operator=(repository_url&&) noexcept = default;

    repository_url(repository_type kind, std::string url, std::string rev = {}) : _url(std::move(url)), _rev(std::move(rev)), _kind(kind)
    {
    }

    [[nodiscard]] constexpr friend bool operator==(repository_url const& lhs, repository_url const& rhs) noexcept = default;

    [[nodiscard]] constexpr friend auto operator<=>(repository_url const& lhs, repository_url const& rhs) noexcept
    {
        if (auto r = lhs.kind() <=> rhs.kind(); r != std::strong_ordering::equal) {
            return r;
        }
        if (auto r = lhs.url() <=> rhs.url(); r != std::strong_ordering::equal) {
            return r;
        }
        return lhs.rev() <=> rhs.rev();
    }

    [[nodiscard]] constexpr std::string const& url() const noexcept
    {
        return _url;
    }

    [[nodiscard]] constexpr std::string const& rev() const noexcept
    {
        return _rev;
    }

    [[nodiscard]] constexpr repository_type kind() const noexcept
    {
        return _kind;
    }

    /** The name of the repository.
     * 
     * The name of the repository is the last path component of the URL excluding
     * the extensions.
     * 
     * @return The name of the repository.
     */
    [[nodiscard]] std::string stem() const;

    /** Sha-256 hash value.
     * 
     * @return A hash calculated from the combined URL and revision.
     */
    [[nodiscard]] std::array<char, 32> hash() const;

    /** Directory name.
     * 
     * The name of the repository together with a hash created from the
     * complete URL and revision.
     */
    [[nodiscard]] std::string directory() const;

private:
    std::string _url = {};
    std::string _rev = {};
    repository_type _kind = repository_type::none;


};


}