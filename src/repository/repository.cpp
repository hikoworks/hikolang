
#include "repository.hpp"
#include "utility/path.hpp"
#include "utility/vector_set.hpp"
#include "utility/file_cursor.hpp"
#include "utility/git.hpp"
#include "error/errors.hpp"
#include "parser/parse_module.hpp"
#include <cassert>
#include <algorithm>
#include <map>

namespace hk {

repository::repository(std::filesystem::path path, repository_url remote) : remote(remote), path(std::move(path))
{
}

void repository::scan_prologues(repository_flags flags)
{
    untouch(false);

    auto first = std::filesystem::recursive_directory_iterator{path};
    auto last = std::filesystem::recursive_directory_iterator{};

    auto visited = vector_set<std::filesystem::path>{};
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

        auto module_path = std::filesystem::canonical(entry.path());
        if (not visited.add(module_path)) {
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

        auto& m = get_module(module_path);
        m.touched = true;

        if (m.ast and m.time == entry.last_write_time() and not to_bool(flags & repository_flags::force_scan)) {
            // The prologue for this module has already been scanned.
            continue;
        }

        auto cursor = file_cursor(module_path);
        if (auto r = parse_module(cursor, true)) {
            m.ast = std::move(r);
            m.time = entry.last_write_time();
        }
    }

    untouch(true);
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
    for (auto &repo : child_repositories()) {
        repo->mark = false;
    }

    while (not todo.empty()) {
        auto [child_remote, import_errors] = std::move(todo.back());
        todo.pop_back();

        auto child_local_path = hkdeps_path / child_remote.directory();
        auto &child_repo = get_child_repository(child_remote, child_local_path);
        if (child_repo.mark) {
            continue;
        }

        child_repo.mark = true;
        if (auto r = git_checkout_or_clone(child_remote, child_local_path, flags); r != git_error::ok) {
            auto short_hkdeps = std::format("_hkdeps/{}", child_remote.directory());
            // TODO #1 All failing import statements of a single repository should be marked with an error.
            import_errors.add(error::could_not_clone_repository, child_remote.url(), child_remote.rev(), short_hkdeps, r);
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

void repository::untouch(bool remove)
{
    auto it = _modules.begin();
    while (it != _modules.end()) {
        if (not it->touched) {
            it = _modules.erase(it, it + 1);
        } else {
            it->touched = false;
            ++it;
        }
    }
}

[[nodiscard]] generator<std::pair<repository_url, error_location>> repository::remote_repositories() const
{
    for (auto const& m : _modules) {
        for (auto repo : m.ast->remote_repositories()) {
            co_yield std::move(repo);
        }
    }
}

[[nodiscard]] repository::module_type& repository::get_module(std::filesystem::path const& module_path)
{
    assert(is_subpath(module_path, path));

    auto it = std::lower_bound(_modules.begin(), _modules.end(), module_path, [](auto const& item, auto const& key) {
        return item.path < key;
    });
    if (it == _modules.end() or it->path != module_path) {
        it = _modules.insert(it, module_type{module_path});
    }
    return *it;
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

repository::module_type::module_type(std::filesystem::path path) : path(std::move(path))
{
    assert(std::filesystem::canonical(this->path) == this->path);
}

} // namespace hk