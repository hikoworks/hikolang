
#include "git.hpp"
#include "defer.hpp"
#include "path.hpp"
#include <git2.h>
#include <mutex>
#include <format>
#include <bit>
#include <print>
#include <algorithm>
#include <utility>
#include <cassert>
#include <exception>

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

    if (auto const it = find_ref(std::format("refs/tags/{}", name)); it != end()) {
        return it;
    }

    return end();
}

static void git_trace_callback(::git_trace_level_t level, const char* msg)
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
[[nodiscard]] static git_lib const& git_lib_initialize()
{
    static auto global = git_lib{};
    return global;
}

[[nodiscard]] constexpr git_error make_git_error(int code)
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

[[nodiscard]] std::expected<git_references, git_error> git_list(std::string const& url)
{
    auto const& _ = git_lib_initialize();

    ::git_remote* remote = nullptr;

    if (auto const r = ::git_remote_create_anonymous(&remote, nullptr, url.c_str()); r != GIT_OK) {
        return std::unexpected{make_git_error(r)};
    }
    auto const d1 = defer{[&] {
        ::git_remote_free(remote);
    }};

    if (auto const r = ::git_remote_connect(remote, GIT_DIRECTION_FETCH, nullptr, nullptr, nullptr); r != GIT_OK) {
        return std::unexpected{make_git_error(r)};
    }

    ::git_remote_head const** list_head = nullptr;
    size_t list_size = 0;

    if (auto const r = ::git_remote_ls(&list_head, &list_size, remote); r != GIT_OK) {
        return std::unexpected{make_git_error(r)};
    }

    auto r = git_references{};
    r.reserve(list_size);

    for (auto i = 0uz; i != list_size; ++i) {
        ::git_remote_head const* entry = list_head[i];

        auto name = std::string{entry->name};

        constexpr auto oid_str_len = sizeof(git_oid) == 20 ? 40 : 64;
        auto oid_str = std::string(oid_str_len, '0');
        ::git_oid_fmt(oid_str.data(), std::addressof(entry->oid));

        r.emplace_back(std::move(name), std::move(oid_str));
    }

    r.sort();
    return r;
}

enum class rev_match {
    /** Rev was not found in the repository.
     */
    rev_not_found,

    /** Rev was not checked out.
     */
    not_checked_out,

    /** Rev was checked out and is a tag or sha.
     */
    checked_out,

    /** Rev was checked out and is a branch.
     */
    checked_out_branch,
};

/** Check if one of the remote has the correct branch checked out.
 *
 * @param repository The repository to check all the remotes
 * @param rev The rev to compare.
 * @retval error An error during query.
 */
[[nodiscard]] static std::expected<rev_match, git_error>
repository_matches_rev(::git_repository* repository, std::string const& rev)
{
    ::git_object* rev_obj = nullptr;
    ::git_reference* rev_ref = nullptr;
    if (auto const result = ::git_revparse_ext(&rev_obj, &rev_ref, repository, rev.c_str()); result != GIT_OK) {
        if (result == GIT_ENOTFOUND) {
            // The rev was not found in the repository.
            return rev_match::rev_not_found;
        }
        return std::unexpected{make_git_error(result)};
    }
    auto const d1 = defer{[&] {
        ::git_object_free(rev_obj);
        ::git_reference_free(rev_ref);
    }};

    ::git_object* peeled_rev_obj = nullptr;
    if (auto const result = ::git_object_peel(&peeled_rev_obj, rev_obj, GIT_OBJECT_COMMIT); result != GIT_OK) {
        return std::unexpected{make_git_error(result)};
    }
    auto const d2 = defer{[&] {
        ::git_object_free(peeled_rev_obj);
    }};

    ::git_reference* head_ref = nullptr;
    if (auto const result = ::git_repository_head(&head_ref, repository); result != GIT_OK) {
        return std::unexpected{make_git_error(result)};
    }
    auto const d3 = defer{[&] {
        ::git_reference_free(head_ref);
    }};

    ::git_object* peeled_head_obj = nullptr;
    if (auto const result = ::git_reference_peel(&peeled_head_obj, head_ref, GIT_OBJECT_COMMIT); result != GIT_OK) {
        return std::unexpected{make_git_error(result)};
    }
    auto const d4 = defer{[&] {
        ::git_object_free(peeled_head_obj);
    }};

    auto const* rev_oid = git_object_id(peeled_rev_obj);
    assert(rev_oid != nullptr);

    auto const* head_oid = git_object_id(peeled_head_obj);
    assert(head_oid != nullptr);

    if (git_oid_cmp(rev_oid, head_oid) != 0) {
        return rev_match::not_checked_out;
    } else if (git_reference_is_branch(rev_ref)) {
        return rev_match::checked_out_branch;
    } else {
        return rev_match::checked_out;
    }
}

/** Check if one of the remote has a url that matches the given url.
 *
 * @param repository The repository to check all the remotes
 * @param remote_name The remote to get the url for.
 * @retval string The url of the remote.
 * @retval error An error during query.
 */
[[nodiscard]] static std::expected<std::string, git_error>
repository_remote_url(::git_repository* repository, std::string const& remote_name = std::string{"origin"})
{
    assert(repository != nullptr);

    ::git_remote* remote = nullptr;
    if (auto const result = ::git_remote_lookup(&remote, repository, remote_name.c_str()); result != GIT_OK) {
        return std::unexpected{make_git_error(result)};
    }
    auto const d1 = defer{[&] {
        ::git_remote_free(remote);
    }};

    auto const remote_url = ::git_remote_url(remote);
    if (remote_url == nullptr) {
        return std::string{""};
    }

    return std::string{remote_url};
}

[[nodiscard]] static git_error
repository_fetch(::git_repository* repository, std::string const& remote_name = std::string{"origin"})
{
    assert(repository != nullptr);

    ::git_remote* remote = nullptr;
    auto fetch_opts = ::git_fetch_options{};

    if (auto result = ::git_fetch_init_options(&fetch_opts, GIT_FETCH_OPTIONS_VERSION); result != 0) {
        return git_error::error;
    }

    if (auto result = ::git_remote_lookup(&remote, repository, remote_name.c_str()); result != GIT_OK) {
        return make_git_error(result);
    }
    auto const d1 = defer{[&] {
        ::git_remote_free(remote);
    }};

    if (auto result = ::git_remote_fetch(remote, nullptr, &fetch_opts, nullptr)) {
        return make_git_error(result);
    }

    return git_error::ok;
}

[[nodiscard]] static std::filesystem::path repository_workdir(::git_repository* repository)
{
    assert(repository != nullptr);

    if (auto* p = ::git_repository_workdir(repository); p != nullptr) {
        return std::filesystem::path{p};
    } else {
        return std::filesystem::path{};
    }
}

/** Clean the repository in preperation for a new checkout.
 *
 * This will remove all untracked and ignored files. It is expected that these
 * are the only files that need to be removed to rebuild the repository.
 *
 * @param repository An open repository.
 * @return An error code or ok.
 */
[[nodiscard]] static git_error repository_clean(::git_repository* repository)
{
    assert(repository != nullptr);

    auto repository_dir = repository_workdir(repository);
    if (repository_dir.empty()) {
        return git_error::bare_repo;
    }
    if (not repository_dir.is_absolute()) {
        return git_error::relative_workdir;
    }
    repository_dir = std::filesystem::canonical(repository_dir);

    auto status_opts = ::git_status_options{};
    if (::git_status_options_init(&status_opts, GIT_STATUS_OPTIONS_VERSION) != 0) {
        return git_error::error;
    }

    status_opts.show = GIT_STATUS_SHOW_WORKDIR_ONLY;
    status_opts.flags |= GIT_STATUS_OPT_INCLUDE_UNTRACKED;
    status_opts.flags |= GIT_STATUS_OPT_INCLUDE_IGNORED;

    ::git_status_list* status_list = nullptr;
    if (auto result = ::git_status_list_new(&status_list, repository, &status_opts); result != GIT_OK) {
        return make_git_error(result);
    }
    auto const d1 = defer{[&] {
        ::git_status_list_free(status_list);
    }};

    auto const count = ::git_status_list_entrycount(status_list);
    for (auto i = 0uz; i != count; ++i) {
        auto const* entry = ::git_status_byindex(status_list, i);
        assert(entry != nullptr);

        if (entry->index_to_workdir != nullptr) {
            if (entry->status == GIT_STATUS_WT_NEW or entry->status == GIT_STATUS_IGNORED) {
                auto path = repository_dir / entry->index_to_workdir->new_file.path;
                path = std::filesystem::canonical(path);

                if (not is_subpath(path, repository_dir)) {
                    std::print(
                        stderr,
                        "security: file '{}' is outside of directory of repository at '{}'",
                        path.string(),
                        repository_dir.string());
                    std::terminate();
                }

                std::print(stderr, "info: removing file '{}'", path.string());

                // The untracked files in untracked directories are not listed,
                // only the directories themselves.
                auto ec = std::error_code{};
                if (not std::filesystem::remove_all(path, ec)) {
                    std::print(stderr, "error: failed removing file '{}': {}", path.string(), ec.message());
                }
            }
        }
    }

    return git_error::ok;
}

[[nodiscard]] static git_error repository_checkout(::git_repository* repository, std::string const& rev)
{
    ::git_object* rev_obj = nullptr;
    ::git_reference* rev_ref = nullptr;
    if (auto const result = ::git_revparse_ext(&rev_obj, &rev_ref, repository, rev.c_str()); result != GIT_OK) {
        if (result == GIT_ENOTFOUND) {
            // The rev was not found in the repository.
            return git_error::not_found;
        }
        return make_git_error(result);
    }
    auto const d1 = defer{[&] {
        ::git_object_free(rev_obj);
        ::git_reference_free(rev_ref);
    }};

    ::git_object* peeled_rev_obj = nullptr;
    if (auto const result = ::git_object_peel(&peeled_rev_obj, rev_obj, GIT_OBJECT_COMMIT); result != GIT_OK) {
        return make_git_error(result);
    }
    auto const d2 = defer{[&] {
        ::git_object_free(peeled_rev_obj);
    }};

    auto checkout_options = ::git_checkout_options{};
    if (::git_checkout_options_init(&checkout_options, GIT_CHECKOUT_OPTIONS_VERSION) != 0) {
        return git_error::error;
    }
    checkout_options.checkout_strategy = GIT_CHECKOUT_FORCE;

    if (auto const result = ::git_checkout_tree(repository, peeled_rev_obj, &checkout_options); result != GIT_OK) {
        return make_git_error(result);
    }

    auto const* commit_oid = ::git_object_id(peeled_rev_obj);
    if (commit_oid == nullptr) {
        return git_error::error;
    }

    if (auto const result = ::git_repository_set_head_detached(repository, commit_oid); result != GIT_OK) {
        return make_git_error(result);
    }

    return git_error::ok;
}

[[nodiscard]] git_error
git_fetch_and_update(std::string const& url, std::string const& rev, std::filesystem::path path, git_checkout_flags flags)
{
    auto const& _ = git_lib_initialize();

    auto r = git_error::ok;

    ::git_repository* repository = nullptr;
    if (auto const result = ::git_repository_open(&repository, path.string().c_str()); result != GIT_OK) {
        return make_git_error(result);
    }
    auto const d1 = defer{[&] {
        ::git_repository_free(repository);
    }};

    if (auto remote_url_o = repository_remote_url(repository)) {
        if (*remote_url_o != url) {
            std::print("The repository at {}, does not have a remote with the url {}", path.string(), url);
            return git_error::remote_url_mismatch;
        }
    } else {
        return remote_url_o.error();
    }

    auto fetch = to_bool(flags & git_checkout_flags::force_fetch);
    if (auto result = repository_matches_rev(repository, rev)) {
        switch (*result) {
        case rev_match::rev_not_found:
            if (to_bool(flags & git_checkout_flags::fresh_clone)) {
                return git_error::rev_not_found;
            }
            [[fallthrough]];
        case rev_match::not_checked_out:
        case rev_match::checked_out_branch:
            fetch |= not to_bool(flags & git_checkout_flags::fresh_clone);
            break;
        case rev_match::checked_out:
            // Revisions that are tags or commits will not cause a fetch.
            break;
        }

    } else {
        return result.error();
    }

    if (fetch) {
        if (auto result = repository_fetch(repository); result != git_error::ok) {
            return result;
        }
    }

    auto checkout = false;
    if (auto result = repository_matches_rev(repository, rev)) {
        switch (*result) {
        case rev_match::rev_not_found:
            return git_error::rev_not_found;
        case rev_match::not_checked_out:
            checkout = true;
            break;
        case rev_match::checked_out:
        case rev_match::checked_out_branch:
            break;
        }
    } else {
        return result.error();
    }

    auto clean = checkout;
    clean |= to_bool(flags & git_checkout_flags::force_clean);
    // A fresh clone does not need to be cleaned.
    clean &= not to_bool(flags & git_checkout_flags::fresh_clone);

    if (clean) {
        if (auto result = repository_clean(repository); result != git_error::ok) {
            return result;
        }
    }

    if (checkout) {
        if (auto result = repository_checkout(repository, rev); result != git_error::ok) {
            return result;
        }
    }

    return git_error::ok;
}

[[nodiscard]] git_error git_clone(std::string const& url, std::string const& git_rev, std::filesystem::path path)
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

    auto branch_it = ref_list.find(git_rev);
    if (branch_it != ref_list.end()) {
        if (branch_it->is_branch()) {
            options.checkout_branch = git_rev.c_str();
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

    ::git_repository* repository = nullptr;
    if (auto r = ::git_clone(&repository, url.c_str(), path.string().c_str(), &options); r != GIT_OK) {
        return make_git_error(r);
    }
    auto d1 = defer{[&] { ::git_repository_free(repository); }};

    return git_error::ok;
}

[[nodiscard]] git_error git_checkout_or_clone(
    std::string const& url, std::string const& rev, std::filesystem::path path, git_checkout_flags flags)
{
    // First try and just update the repository.
    switch(git_fetch_and_update(url, rev, path, flags)) {
    case git_error::ok:
        return git_error::ok;

    case git_error::rev_not_found:
        // Repository was found but not the ref.
        return git_error::rev_not_found;

    case git_error::not_found:
        // The repository was not found, try and clone instead.
        break;

    default:
        return git_error::error;
    }

    if (auto r = git_clone(url, rev, path); r != git_error::ok) {
        return r;
    }

    // In case rev is a tag or commit, checkout/update the repository.
    flags |= git_checkout_flags::fresh_clone;
    return git_fetch_and_update(url, rev, path, flags);
}

} // namespace hk