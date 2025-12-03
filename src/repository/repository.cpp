
#include "repository.hpp"
#include "utility/path.hpp"
#include "utility/vector_set.hpp"
#include "utility/git.hpp"
#include "parser/parse_top.hpp"
#include <cassert>
#include <algorithm>
#include <map>
#include <print>

namespace hk {

repository::repository(std::filesystem::path path, repository_url remote) : remote(remote), path(std::move(path)) {}

void repository::scan_prologues(repository_flags flags)
{
    gather_modules();
    parse_prologues(flags);
    sort_modules();
}

bool repository::gather_modules()
{
    auto first = std::filesystem::recursive_directory_iterator{path};
    auto last = std::filesystem::recursive_directory_iterator{};

    // Keep track of modules that we have (partially) compiled.
    // So that we can remove modules that no longer exist on the filesystem.
    auto modified = false;
    auto existing_source_paths = vector_set<std::reference_wrapper<std::filesystem::path const>>{};
    for (auto& m : _sources_by_path) {
        if (not m->is_generated()) {
            existing_source_paths.add(m->path());
        }
    }

    auto visited_directories = vector_set<std::filesystem::path>{};
    for (auto it = first; it != last; ++it) {
        auto const& entry = *it;

        auto const start_of_filename = entry.path().filename().string()[0];
        if (start_of_filename == '.' or start_of_filename == '_') {
            // Do not recursively scan hidden directories.
            if (entry.is_directory()) {
                it.disable_recursion_pending();
            }
            continue;
        }

        auto source_path = std::filesystem::canonical(entry.path());
        if (not visited_directories.add(source_path)) {
            // This directory has already been visited; a potential symlink loop.
            if (entry.is_directory()) {
                it.disable_recursion_pending();
            }
            continue;
        }

        if (not entry.is_regular_file()) {
            continue;
        }

        if (entry.path().extension() != ".hkm") {
            continue;
        }

        if (not existing_source_paths.contains_and_erase(source_path)) {
            // The module did not exist yet.
            modified = true;

            auto it =
                std::lower_bound(_sources_by_path.begin(), _sources_by_path.end(), source_path, [](auto const& e, auto const& p) {
                    if (e->is_generated()) {
                        return true;
                    } else {
                        return e->path() < p;
                    }
                });
            assert(it == _sources_by_path.end() or (*it)->is_generated() or (*it)->path() != source_path);
            _sources_by_path.insert(it, std::make_unique<source>(*this, source_path));
        }
    }

    // Remove any modules where the on-disk file is missing.
    auto it = _sources_by_path.begin();
    for (auto const& p : existing_source_paths) {
        modified = true;

        it = std::lower_bound(it, _sources_by_path.end(), p, [](auto const& e, auto const& p) {
            if (e->is_generated()) {
                return true;
            } else {
                return e->path() < p;
            }
        });
        assert(it != _sources_by_path.end() and not(*it)->is_generated() and (*it)->path() == p);
        it = _sources_by_path.erase(it);
    }

    return modified;
}

void repository::sort_modules() {}

bool repository::parse_prologues(repository_flags flags)
{
    auto modified = false;

    // It is expected that _modules is sorted in compilation order,
    // it is also possible for the file to be removed.
    auto it = _sources_by_path.begin();
    while (it != _sources_by_path.end()) {
        if ((*it)->is_generated()) {
            // prologues of generated files are handled during compilation.
            ++it;
            continue;
        }

        if (auto r = (*it)->parse_prologue(); not r) {
            std::println(stderr, "Could not get prologue of file '{}': {}", (*it)->path().string(), r.error().message());
            ++it;
            continue;
        }
        modified = true;
    }

    return modified;
}

void repository::recursive_scan_prologues(repository_flags flags)
{
    auto todo = std::vector<std::pair<repository_url, error_location>>{};

    scan_prologues(flags);
    for (auto item : remote_repositories()) {
        todo.push_back(std::move(item));
    }

    auto hkdeps_path = path / "_hkdeps";

    auto ec = std::error_code{};
    if (not std::filesystem::create_directory(hkdeps_path, ec) and ec) {
        std::println(stderr, "Error: could not create directory '{}': {}.", hkdeps_path.string(), ec.message());
        std::terminate();
    }

    // The mark will be used to see if a child repository was already processed.
    for (auto& repo : child_repositories()) {
        repo->mark = false;
    }

    while (not todo.empty()) {
        auto [child_remote, import_errors] = std::move(todo.back());
        todo.pop_back();

        auto child_local_path = hkdeps_path / child_remote.directory();
        auto& child_repo = get_child_repository(child_remote, child_local_path);
        if (child_repo.mark) {
            continue;
        }

        child_repo.mark = true;
        if (auto r = git_checkout_or_clone(child_remote, child_local_path, flags); r != git_error::ok) {
            auto short_hkdeps = std::format("_hkdeps/{}", child_remote.directory());
            // TODO #1 All failing import statements of a single repository should be marked with an error.
            import_errors.add(
                hkc_error::could_not_clone_repository,
                "git-url: {}, rev: {}, dir: .hkdeps/{}, error: {}",
                child_remote.url(),
                child_remote.rev(),
                short_hkdeps,
                r);
            erase_child_repository(child_remote);
            continue;
        }

        child_repo.scan_prologues(flags);
        for (auto item : child_repo.remote_repositories()) {
            todo.push_back(std::move(item));
        }
    }

    // Remove internal repositories not in 'done'.
    std::erase_if(_child_repositories, [&](auto const& item) {
        return not item->mark;
    });
}

[[nodiscard]] generator<std::pair<repository_url, error_location>> repository::remote_repositories() const
{
    for (auto const& m : _modules) {
        for (auto const& remote_repository : m.ast->remote_repositories) {
            auto const el = error_location{m.errors, remote_repository->first, remote_repository->last};
            co_yield std::pair{remote_repository->url, el};
        }
    }
}

[[nodiscard]] repository& repository::get_child_repository(repository_url const& remote, std::filesystem::path child_path)
{
    auto it =
        std::lower_bound(_child_repositories.begin(), _child_repositories.end(), remote, [](auto const& item, auto const& key) {
            return item->remote < key;
        });
    if (it == _child_repositories.end() or (*it)->remote != remote) {
        it = _child_repositories.insert(it, std::make_unique<repository>(child_path, remote));
    }
    return **it;
}

void repository::erase_child_repository(repository_url const& remote)
{
    auto it =
        std::lower_bound(_child_repositories.begin(), _child_repositories.end(), remote, [](auto const& item, auto const& key) {
            return item->remote < key;
        });

    if (it != _child_repositories.end() and (*it)->remote == remote) {
        _child_repositories.erase(it);
    }
}

} // namespace hk