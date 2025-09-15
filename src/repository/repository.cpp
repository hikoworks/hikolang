
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

repository::repository(std::filesystem::path path) : _path(std::move(path))
{
    assert(std::filesystem::canonical(_path) == _path);
}

void repository::scan_prologues(repository_flags flags)
{
    untouch(false);

    auto first = std::filesystem::recursive_directory_iterator{_path};
    auto last = std::filesystem::recursive_directory_iterator{};

    auto visited = vector_set<std::filesystem::path>{};
    for (auto it = first; it != last; ++it) {
        auto const& entry = *it;

        if (entry.path().filename().string().starts_with(".")) {
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

error_code repository::recursive_scan_prologues(repository_flags flags)
{
    auto todo = std::map<repository_url, error_location>{};
    auto done = std::map<repository_url, error_location>{};

    scan_prologues(flags);
    for (auto item : remote_repositories()) {
        todo.insert(std::move(item));
    }

    auto hkdeps_path = _path / "_hkdeps";

    auto ec = std::error_code{};
    if (not std::filesystem::create_directory(hkdeps_path, ec) and ec) {
        std::println(stderr, "Error: could not create directory '{}': {}.", hkdeps_path.string(), ec.message());
        std::terminate();
    }

    while (not todo.empty()) {
        auto child_repo_url_node = todo.extract(todo.begin());
        auto [it, inserted, _] = done.insert(std::move(child_repo_url_node));
        if (not inserted) {
            continue;
        }

        assert(it != done.end());
        auto child_repo_path = hkdeps_path / it->first.directory();
        auto &child_repo = get_child_repository(it->first);
        if (not child_repo.repository) {
            if (auto r = git_checkout_or_clone(it->first, child_repo_path, flags); r != git_error::ok) {
                auto short_hkdeps = std::format("_hkdeps/{}", it->first.directory());
                return it->second.add(error::could_not_clone_repository, it->first.url(), it->first.rev(), short_hkdeps, r).error();
            }

            child_repo.repository = std::make_unique<repository>(child_repo_path);
            child_repo.repository->scan_prologues(flags);
            for (auto item : child_repo.repository->remote_repositories()) {
                todo.insert(std::move(item));
            }
        }
    }

    // Remove internal repositories not in 'done'.
    std::erase_if(_child_repositories, [&](auto const& item) {
        return not done.contains(item.url);
    });

    return hk::error_code{};
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
    assert(is_subpath(module_path, _path));

    auto it = std::lower_bound(_modules.begin(), _modules.end(), module_path, [](auto const& item, auto const& key) {
        return item.path < key;
    });
    if (it == _modules.end() or it->path != module_path) {
        it = _modules.insert(it, module_type{module_path});
    }
    return *it;
}

[[nodiscard]] repository::child_repository_type& repository::get_child_repository(repository_url const& url)
{
    auto it =
        std::lower_bound(_child_repositories.begin(), _child_repositories.end(), url, [](auto const& item, auto const& key) {
            return item.url < key;
        });
    if (it == _child_repositories.end() or it->url != url) {
        it = _child_repositories.insert(it, child_repository_type{url});
    }
    return *it;
}

repository::module_type::module_type(std::filesystem::path path) : path(std::move(path))
{
    assert(std::filesystem::canonical(this->path) == this->path);
}

} // namespace hk