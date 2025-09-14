
#include "repository.hpp"
#include "utility/path.hpp"
#include "utility/vector_set.hpp"
#include "utility/file_cursor.hpp"
#include "utility/git.hpp"
#include "parser/parse_module.hpp"
#include <cassert>
#include <algorithm>

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

void repository::recursive_scan_prologues(repository_flags flags)
{
    auto todo = std::set<repository_url>{};
    auto done = std::set<repository_url>{};

    scan_prologues(flags);
    for (auto child_repo_url : remote_repositories()) {
        todo.insert(std::move(child_repo_url));
    }

    auto hkdeps = _path / "_hkdeps";
    while (not todo.empty()) {
        auto child_repo_url_node = todo.extract(todo.begin());
        auto [it, inserted, _] = done.insert(std::move(child_repo_url_node));
        if (not inserted) {
            continue;
        }

        assert(it != done.end());
        auto child_repo_path = hkdeps / it->directory();
        auto &child_repo = get_child_repository(*it);
        if (not child_repo.repository) {
            if (auto r = git_checkout_or_clone(*it, child_repo_path, flags); r != git_error::ok) {

            }

            child_repo.repository = std::make_unique<repository>(child_repo_path);
        }
    }

    // Remove internal repositories not in done.
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

[[nodiscard]] generator<repository_url> repository::remote_repositories() const
{
    for (auto const& m : _modules) {
        for (auto u : m.ast->remote_repositories()) {
            co_yield std::move(u);
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