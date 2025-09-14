
#include "git_error.hpp"
#include <git2.h>
#include <cassert>
#include <utility>
#include <string>

namespace hk {

[[nodiscard]] git_error make_git_error(int code)
{
    switch (code) {
    case GIT_OK:
        return git_error::ok;
    case GIT_ERROR:
        return git_error::error;
    case GIT_ENOTFOUND:
        return git_error::not_found;
    case GIT_EEXISTS:
        return git_error::exists;
    case GIT_EAMBIGUOUS:
        return git_error::ambiguous;
    case GIT_EBUFS:
        return git_error::bufs;
    case GIT_EUSER:
        return git_error::user;
    case GIT_EBAREREPO:
        return git_error::bare_repo;
    case GIT_EUNBORNBRANCH:
        return git_error::unborn_branch;
    case GIT_EUNMERGED:
        return git_error::unmerged;
    case GIT_ENONFASTFORWARD:
        return git_error::non_fast_forward;
    case GIT_EINVALIDSPEC:
        return git_error::invalid_spec;
    case GIT_ECONFLICT:
        return git_error::conflict;
    case GIT_ELOCKED:
        return git_error::locked;
    case GIT_EMODIFIED:
        return git_error::modified;
    case GIT_EAUTH:
        return git_error::auth;
    case GIT_ECERTIFICATE:
        return git_error::certificate;
    case GIT_EAPPLIED:
        return git_error::applied;
    case GIT_EPEEL:
        return git_error::peel;
    case GIT_EEOF:
        return git_error::eof;
    case GIT_EINVALID:
        return git_error::invalid;
    case GIT_EUNCOMMITTED:
        return git_error::uncommitted;
    case GIT_EDIRECTORY:
        return git_error::directory;
    case GIT_EMERGECONFLICT:
        return git_error::merge_conflict;
    case GIT_PASSTHROUGH:
        return git_error::pass_through;
    case GIT_ITEROVER:
        return git_error::iter_over;
    case GIT_RETRY:
        return git_error::retry;
    case GIT_EMISMATCH:
        return git_error::mismatch;
    case GIT_EINDEXDIRTY:
        return git_error::index_dirty;
    case GIT_EAPPLYFAIL:
        return git_error::apply_fail;
    case GIT_EOWNER:
        return git_error::owner;
    case GIT_TIMEOUT:
        return git_error::timeout;
    case GIT_EUNCHANGED:
        return git_error::unchanged;
    case GIT_ENOTSUPPORTED:
        return git_error::not_supported;
    case GIT_EREADONLY:
        return git_error::read_only;
    default:
        throw std::runtime_error(std::format("Unknown git error code {}", code));
    }
}

[[nodiscard]] std::string to_string(git_error error)
{
    using namespace std::literals;

    switch (error) {
    case git_error::ok:
        assert(false);
    case git_error::error:
        return "git: Unknown error."s;
    case git_error::not_found:
        return "git: Not found."s;
    case git_error::exists:
        return "git: Exists."s;
    case git_error::ambiguous:
        return "git: Ambiguous."s;
    case git_error::bufs:
        return "git: Buffers."s;
    case git_error::user:
        return "git: User."s;
    case git_error::bare_repo:
        return "git: Unexpected bare-repo."s;
    case git_error::unborn_branch:
        return "git: Unborn branch."s;
    case git_error::unmerged:
        return "git: Unmerged changes."s;
    case git_error::non_fast_forward:
        return "git: Non fast forward."s;
    case git_error::invalid_spec:
        return "git: Invalid spec."s;
    case git_error::conflict:
        return "git: Update conflicts."s;
    case git_error::locked:
        return "git: Locked."s;
    case git_error::modified:
        return "git: Modified."s;
    case git_error::auth:
        return "git: Failed to authenticate."s;
    case git_error::certificate:
        return "git: Incorrect certificate."s;
    case git_error::applied:
        return "git: Applied."s;
    case git_error::peel:
        return "git: Peel."s;
    case git_error::eof:
        return "git: End of file."s;
    case git_error::invalid:
        return "git: Invalid."s;
    case git_error::uncommitted:
        return "git: There are uncommitted changes"s;
    case git_error::directory:
        return "git: Directory."s;
    case git_error::merge_conflict:
        return "git: Merge conflict."s;
    case git_error::pass_through:
        return "git: Pass through."s;
    case git_error::iter_over:
        return "git: Iter over."s;
    case git_error::retry:
        return "git: Retry."s;
    case git_error::mismatch:
        return "git: Mismatch."s;
    case git_error::index_dirty:
        return "git: Index dirty"s;
    case git_error::apply_fail:
        return "git: Apply fail."s;
    case git_error::owner:
        return "git: Owner."s;
    case git_error::timeout:
        return "git: Timeout."s;
    case git_error::unchanged:
        return "git: Unchanged."s;
    case git_error::not_supported:
        return "git: Not supported."s;
    case git_error::read_only:
        return "git: Read only"s;
    case git_error::rev_not_found:
        return "git: Could not find rev in repository."s;
    case git_error::remote_url_mismatch:
        return "git: cloned repositery's remote url mismatches with given url."s;
    case git_error::relative_workdir:
        return "git: The cloned repository's work directory is relative."s;
    case git_error::file_outside_workdir:
        return "git: Trying to clean/remove a file outside of the cloned repository's work directory."s;
    case git_error::could_not_connect_with_remote:
        return "git: Could not connect with remote repository."s;
    }
    std::unreachable();
}

}
