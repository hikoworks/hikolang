
#include "git.hpp"
#include "defer.hpp"
#include <git2.h>
#include <mutex>
#include <format>
#include <bit>
#include <print>
#include <algorithm>
#include <utility>
#include <cassert>

namespace hk {

void git_references::sort() noexcept
{
    std::sort(begin(), end());
}

[[nodiscard]] git_references::const_iterator git_references::find_ref(std::string const& ref_name) const
{
    auto it = std::lower_bound(begin(), end(), ref_name, [](auto const& a, auto const& b) {
        return a.name < b;
    });
    if (it == end() or it->name != ref_name) {
        return end();
    }
    return it;
}

[[nodiscard]] git_references::const_iterator git_references::find(std::string_view name) const
{
    if (auto const it = find_ref(std::format("refs/heads/{}", name)); it != end()) {
        return it;
    }

    if (auto const it = find_ref(std::format("ref/tags/{}", name)); it != end()) {
        return it;
    }



    return end();
}

static void git_trace_callback(::git_trace_level_t level, const char *msg)
{
    std::print("GIT TRACE: {}", msg);
}

class git_lib {
public:
    ~git_lib()
    {
        ::git_libgit2_shutdown();
        //::git_trace_set(GIT_TRACE_DEBUG, git_trace_callback);
    }

    git_lib()
    {
        ::git_libgit2_init();
    }
};

/** Initialize libgit2 library.
 * 
 * If the library is initialized, it will be shutdown after main().
 */
[[nodiscard]] static git_lib const &git_lib_initialize()
{
    static auto global = git_lib{};
    return global;
}

[[nodiscard]] constexpr git_error make_git_error(int code)
{
    switch (code) {
    case GIT_OK: return git_error::ok;
    case GIT_ERROR: return git_error::error;
    case GIT_ENOTFOUND: return git_error::not_found;
    case GIT_EEXISTS: return git_error::exists;
    case GIT_EAMBIGUOUS: return git_error::ambiguous;
    case GIT_EBUFS: return git_error::bufs;
    case GIT_EUSER: return git_error::user;
    case GIT_EBAREREPO: return git_error::bare_repo;
    case GIT_EUNBORNBRANCH: return git_error::unborn_branch;
    case GIT_EUNMERGED: return git_error::unmerged;
    case GIT_ENONFASTFORWARD: return git_error::non_fast_forward;
    case GIT_EINVALIDSPEC: return git_error::invalid_spec;
    case GIT_ECONFLICT: return git_error::conflict;
    case GIT_ELOCKED: return git_error::locked;
    case GIT_EMODIFIED: return git_error::modified;
    case GIT_EAUTH: return git_error::auth;
    case GIT_ECERTIFICATE: return git_error::certificate;
    case GIT_EAPPLIED: return git_error::applied;
    case GIT_EPEEL: return git_error::peel;
    case GIT_EEOF: return git_error::eof;
    case GIT_EINVALID: return git_error::invalid;
    case GIT_EUNCOMMITTED: return git_error::uncommitted;
    case GIT_EDIRECTORY: return git_error::directory;
    case GIT_EMERGECONFLICT: return git_error::merge_conflict;
    case GIT_PASSTHROUGH: return git_error::pass_through;
    case GIT_ITEROVER: return git_error::iter_over;
    case GIT_RETRY: return git_error::retry;
    case GIT_EMISMATCH: return git_error::mismatch;
    case GIT_EINDEXDIRTY: return git_error::index_dirty;
    case GIT_EAPPLYFAIL: return git_error::apply_fail;
    case GIT_EOWNER: return git_error::owner;
    case GIT_TIMEOUT: return git_error::timeout;
    case GIT_EUNCHANGED: return git_error::unchanged;
    case GIT_ENOTSUPPORTED: return git_error::not_supported;
    case GIT_EREADONLY: return git_error::read_only;
    default:
        throw std::runtime_error(std::format("Unknown git error code {}", code));
    }
}



[[nodiscard]] std::expected<git_references, git_error> git_list(std::string const& url)
{
    auto const& _ = git_lib_initialize();

    ::git_remote *remote = nullptr;

    if (auto const r = ::git_remote_create_anonymous(&remote, nullptr, url.c_str()); r != GIT_OK) {
        return std::unexpected{make_git_error(r)};
    }

    if (auto const r = ::git_remote_connect(remote, GIT_DIRECTION_FETCH, nullptr, nullptr, nullptr); r != GIT_OK) {
        ::git_remote_free(remote);
        return std::unexpected{make_git_error(r)};
    }

    ::git_remote_head const **list_head = nullptr;
    size_t list_size = 0;

    if (auto const r = ::git_remote_ls(&list_head, &list_size, remote); r != GIT_OK) {
        ::git_remote_free(remote);
        return std::unexpected{make_git_error(r)};
    }

    auto r = git_references{};
    r.reserve(list_size);

    for (auto i = 0uz; i != list_size; ++i) {
        ::git_remote_head const *entry = list_head[i];

        auto name = std::string{entry->name};

        constexpr auto oid_str_len = sizeof(git_oid) == 20 ? 40 : 64;
        auto oid_str = std::string(oid_str_len, '0');
        ::git_oid_fmt(oid_str.data(), std::addressof(entry->oid));

        r.emplace_back(std::move(name), std::move(oid_str));
    }

    ::git_remote_free(remote);
    r.sort();
    return r;
}

/** Check if one of the remote has the correct branch checked out.
 * 
 * @param repository The repository to check all the remotes
 * @param branch The branch to compare.
 * @retval true The correct branch was checked out.
 * @retval false Another branch was checked out.
 * @retval error An error during query.
 */
[[nodiscard]] std::expected<bool, git_error> repository_matches_branch(::git_repository *repository, std::string const& branch)
{

}

/** Check if one of the remote has a url that matches the given url.
 * 
 * @param repository The repository to check all the remotes
 * @param url The url to compare.
 * @retval true The url is a remote of the @a repository.
 * @retval false None of the remotes match.
 * @retval error An error during query.
 */
[[nodiscard]] std::expected<bool, git_error> repository_matches_url(::git_repository *repository, std::string const& url)
{
    assert(repository != nullptr);

    auto remote_names = ::git_strarray{};
    if (auto const result = ::git_remote_list(&remote_names, repository); result != GIT_OK) {
        return std::unexpected{make_git_error(result)};
    }
    auto const _ = defer{[&]{ ::git_strarray_dispose(&remote_names); }};

    for (auto i = 0uz; i != remote_names.count; ++i) {
        ::git_remote *remote = nullptr;
        if (auto const result = ::git_remote_lookup(&remote, repository, remote_names.strings[i]); result != GIT_OK) {
            return std::unexpected{make_git_error(result)};
        }
        auto const _ = defer{[&]{ ::git_remote_free(remote); }};

        auto const remote_url = ::git_remote_url(remote);
        if (remote_url and url == remote_url) {
            return true;
        }
    }

    return false;
}

[[nodiscard]] git_error git_fetch_and_update(std::string const& url, std::string const& branch, std::filesystem::path path, git_checkout_flags flags)
{
    auto const& _ = git_lib_initialize();

    auto r = git_error::ok;

    ::git_repository *repository = nullptr;
    if (auto const result = ::git_repository_open(&repository, url.c_str()); result != GIT_OK) {
        return make_git_error(result);
    }
    auto const d1 = defer{[&]{ ::git_repository_free(repository); }};

    if (auto result = repository_matches_url(repository, url)) {
        if (not *result) {
            std::print("The repository at {}, does not have a remote with the url {}", path.string(), url);
            return git_error::remote_url_mismatch;
        }
    } else {
        return result.error();
    }

    if (auto result = repository_matches_branch(repository, branch)) {

    }
}

[[nodiscard]] git_error git_clone(std::string const& url, std::string const& branch, std::filesystem::path path)
{
    auto const& _ = git_lib_initialize();


    auto force_checkout = false;

    auto ref_list = git_references{};
    if (auto ref_list_o = git_list(url)) {
        ref_list = std::move(ref_list_o).value();
    } else {
        return ref_list_o.error();
    }

    ::git_clone_options options;
    if (auto r = git_clone_options_init(&options, GIT_CLONE_OPTIONS_VERSION); r != GIT_OK) {
        return make_git_error(r);
    }

    auto branch_it = ref_list.find(branch);
    if (branch_it != ref_list.end()) {
        if (branch_it->is_branch()) {
            options.checkout_branch = branch.c_str();
            options.fetch_opts.depth = 1;
            force_checkout = true;

        } else if (branch_it->is_tag()) {
            // Tags need to be checkout after clone.

        } else {
            assert(false);
        }

    } else {
        // Expect branch to be a hex-sha.
    }

    ::git_repository *repository = nullptr;
    if (auto r = ::git_clone(&repository, url.c_str(), path.string().c_str(), &options); r != GIT_OK) {
        return make_git_error(r);
    }
}

//[[nodiscard]] git_error git_checkout_or_clone(
//    std::string const& url, std::string const& branch, std::filesystem::path path, git_checkout_flags flags)
//{
//    if (git_is_checked_out(path)) {
//        // Check if the repository should be checked out again.
//
//        return git_error::ok;
//    }
//
//    if (git_is_branch(url, branch)) {
//        git_clone_shallow(url, branch, path);
//
//    } else if (git_is_tag(url, branch)) {
//        git_clone(url, path);
//        git_checkout(path, branch);
//
//    } else {
//        git_clone(url, path);
//        git_checkout_oid(path, branch);
// 
//    }
//
//}


}