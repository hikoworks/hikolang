
#pragma once

#include <format>

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

    /** The revision was not found in this repository.
     */
    rev_not_found,

    /** The repository does have a remote that matches the expected URL.
     */
    remote_url_mismatch,

    /** The workdir of the repository is relative, instead of absolute.
     */
    relative_workdir,

    /** Found a file outside of the work dir.
     *
     * This is a security issue.
     */
    file_outside_workdir,

    /** Could not connect with remote server using the URL passed.
     */
    could_not_connect_with_remote,
};

[[nodiscard]] git_error make_git_error(int code);
[[nodiscard]] std::string to_string(git_error error);

}

template<>
struct std::formatter<hk::git_error, char> : public std::formatter<std::string, char> {
    template<class FmtContext>
    FmtContext::iterator format(hk::git_error e, FmtContext& ctx) const
    {
        auto msg = to_string(e);
        return std::formatter<std::string, char>::format(msg, ctx);
    }
};

