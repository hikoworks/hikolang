
#pragma once

#include "repository_url.hpp"
#include "repository_flags.hpp"
#include "git_error.hpp"
#include <expected>
#include <system_error>
#include <memory>
#include <cstdint>
#include <cstddef>
#include <array>
#include <vector>
#include <string>
#include <filesystem>
#include <utility>
#include <cassert>

namespace hk {




struct git_reference {
    std::string name = {};
    std::string oid = {};

    [[nodiscard]] constexpr friend bool operator==(git_reference const& lhs, git_reference const& rhs) noexcept
    {
        return lhs.name == rhs.name;
    }

    [[nodiscard]] constexpr friend auto operator<=>(git_reference const& lhs, git_reference const& rhs) noexcept
    {
        return lhs.name <=> rhs.name;
    }

    [[nodiscard]] constexpr bool is_branch() const noexcept
    {
        return name.starts_with("refs/heads/");
    }

    [[nodiscard]] constexpr bool is_tag() const noexcept
    {
        return name.starts_with("refs/tags/");
    }
};

class git_references : public std::vector<git_reference> {
public:
    using std::vector<git_reference>::vector;

    /** Find a specific ref.
     *
     * @pre sort() must be called first.
     * @param ref_name The full ref-name.
     * @return An iterator to the reference, or end() if not found.
     */
    [[nodiscard]] const_iterator find_ref(std::string const& ref_name) const;

    /** Find a name.
     *
     * @pre sort() must be called first.
     * @param name A branch, tag or hex-oid.
     * @return An iterator to the reference, or end() if not found.
     */
    [[nodiscard]] const_iterator find(std::string_view name) const;

    /** Git references alphabetical
     */
    void sort() noexcept;
};

/** Get a list of references.
 *
 * @param url URL to remote repository
 * @return List of references, or error.
 */
[[nodiscard]] std::expected<git_references, git_error> git_list(std::string const& url);

/** Clone a repository.
 *
 * @param url The (remote) location of the repository
 * @param git_rev The branch to checkout.
 *                If git_rev is a branch the checkout is done with depth 1.
 *                If git_rev is a tag then the default branch is checked with
 *                full depth.
 */
[[nodiscard]] git_error git_clone(std::string const& url, std::string const& git_rev, std::filesystem::path path);

/** This function will open the repository and update to the latest version.
 *
 * @param url The remote url, used to check if the repository at the path
 *            has the same remote url.
 * @param rev The rev (branch/tag/sha) to checkout. If the repository is of a different
 *            rev this branch is checked out, and the repository is cleaned.
 * @param path The path where the repository is located.
 * @param flags Flags for the way the repository should be checked out.
 */
[[nodiscard]] git_error git_fetch_and_update(
    std::string const& url,
    std::string const& rev,
    std::filesystem::path path,
    repository_flags flags = repository_flags{});

/** Checkout or clone the repository.
 *
 * @param url The location of the remote repository.
 * @param rev The rev (branch/tag/sha) to checkout. If the repository is of a different
 *            rev this branch is checked out, and the repository is cleaned.
 * @param path The location where to clone/checkout the repository to.
 * @param flags Flags for what to do with an already cloned repository.
 * @return An error, or git_error::ok.
 */
[[nodiscard]] git_error git_checkout_or_clone(
    std::string const& url,
    std::string const& rev,
    std::filesystem::path path,
    repository_flags flags = repository_flags{});

/** Checkout or clone the repository.
 *
 * @see git_checkout_or_clone
 */
[[nodiscard]] inline git_error
git_checkout_or_clone(repository_url const& url, std::filesystem::path path, repository_flags flags = repository_flags{})
{
    assert(url.kind() == repository_type::git);
    return git_checkout_or_clone(url.url(), url.rev(), path, flags);
}

} // namespace hk
