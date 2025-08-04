
#pragma once

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

namespace hk {

enum class git_error {
    ok = 0,
    error,
    not_found,
    exists,
    ambiguous,
    bufs,
    user,
    bare_repo,
    unborn_branch,
    unmerged,
    non_fast_forward,
    invalid_spec,
    conflict,
    locked,
    modified,
    auth,
    certificate,
    applied,
    peel,
    eof,
    invalid,
    uncommitted,
    directory,
    merge_conflict,
    pass_through,
    iter_over,
    retry,
    mismatch,
    index_dirty,
    apply_fail,
    owner,
    timeout,
    unchanged,
    not_supported,
    read_only,
};

enum class git_checkout_flags {
    force_checkout = 0x1,
    clean = 0x2
};

[[nodiscard]] constexpr git_checkout_flags operator|(git_checkout_flags lhs, git_checkout_flags rhs) noexcept
{
    return static_cast<git_checkout_flags>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

[[nodiscard]] constexpr git_checkout_flags operator&(git_checkout_flags lhs, git_checkout_flags rhs) noexcept
{
    return static_cast<git_checkout_flags>(std::to_underlying(lhs) & std::to_underlying(rhs));
}

[[nodiscard]] constexpr bool to_bool(git_checkout_flags rhs) noexcept
{
    return static_cast<bool>(std::to_underlying(rhs));
}

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

[[nodiscard]] git_error git_clone(std::string const& url, std::string const& branch, std::filesystem::path path);

[[nodiscard]] git_error git_checkout(std::filesystem::path path);

/** Checkout or clone the repository.
 * 
 * @param url The location of the remote repository.
 * @param branch The branch, tag, or hex-oid
 * @param path The location where to clone/checkout the repository to.
 * @param flags Flags for what to do with an already cloned repository.
 * @return An error, or git_error::ok.
 */
[[nodiscard]] git_error git_checkout_or_clone(
    std::string const& url, std::string const& branch, std::filesystem::path path, git_checkout_flags flags);

}
