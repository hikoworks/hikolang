
#pragma once

#include <string>

namespace hk {

class remote_repo_url {
public:
    enum class kind_type {
        empty,
        git,
        zip,
    };

    constexpr static kind_type empty = kind_type::empty;
    constexpr static kind_type git = kind_type::empty;
    constexpr static kind_type zip = kind_type::empty;

    constexpr remote_repo_url() noexcept = default;
    constexpr remote_repo_url(remote_repo_url const&) noexcept = default;
    constexpr remote_repo_url(remote_repo_url&&) noexcept = default;
    constexpr remote_repo_url& operator=(remote_repo_url const&) noexcept = default;
    constexpr remote_repo_url& operator=(remote_repo_url&&) noexcept = default;

    remote_repo_url(kind_type kind, std::string url, std::string rev = {}) : _url(std::move(url)), _rev(std::move(rev)), _kind(kind)
    {
    }

    [[nodiscard]] constexpr std::string const& url() const noexcept
    {
        return _url;
    }

    [[nodiscard]] constexpr std::string const& rev() const noexcept
    {
        return _rev;
    }

    [[nodiscard]] constexpr kind_type kind() const noexcept
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
    [[nodiscard]] std::string hash() const;

    /** Directory name.
     * 
     * The name of the repository together with a hash created from the
     * complete URL and revision.
     */
    [[nodiscard]] std::string directory() const;

private:
    std::string _url = {};
    std::string _rev = {};
    kind_type _kind = kind_type::empty;


};


}