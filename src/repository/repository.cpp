
#include "repository.hpp"
#include "utility/path.hpp"
#include "utility/vector_set.hpp"
#include "tokenizer/file_cursor.hpp"
#include "utility/git.hpp"
#include "error/errors.hpp"
#include "parser/parse_top.hpp"
#include <cassert>
#include <algorithm>
#include <map>

namespace hk {

repository::repository(std::filesystem::path path, repository_url remote) : remote(remote), path(std::move(path)) {}

void repository::scan_prologues(repository_flags flags)
{
    auto must_sort = gather_modules();

    auto ctx = parse_context{};
    must_sort |= parse_modules(ctx, module_t::state_type::prologue, flags);

    if (must_sort) {
        sort_modules();
    }
}

bool repository::gather_modules()
{
    auto first = std::filesystem::recursive_directory_iterator{path};
    auto last = std::filesystem::recursive_directory_iterator{};

    // Keep track of modules that we have (partially) compiled.
    // So that we can remove modules that no longer exist on the filesystem.
    auto modified = false;
    auto existing_module_paths = vector_set<std::filesystem::path>{};
    for (auto& m : _modules) {
        existing_module_paths.add(m.path);
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

        auto module_path = std::filesystem::canonical(entry.path());
        if (not visited_directories.add(module_path)) {
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

        if (not existing_module_paths.contains_and_erase(module_path)) {
            // The module did not exist yet.
            _modules.insert(module_path);
            modified = true;
        }
    }

    // Remove any modules where the on-disk file is missing.
    for (auto const& p : existing_module_paths) {
        _modules.erase(p);
    }

    return modified;
}

void repository::sort_modules()
{

}

bool repository::parse_modules(parse_context& ctx, module_t::state_type new_state, repository_flags flags)
{
    assert(new_state != module_t::state_type::out_of_date);

    auto modified = false;

    // It is expected that _modules is sorted in compilation order,
    // it is also possible for the file to be removed.
    auto it = _modules.begin();
    while (it != _modules.end()) {
        auto ec = std::error_code{};
        auto const last_write_time = std::filesystem::last_write_time(it->path, ec);
        if (ec) {
            std::println(stderr, "Could not get date from file `{}`: {}", it->path.string(), ec.message());
            it = _modules.erase(it);
            continue;
        }

        if (to_bool(flags & repository_flags::force_scan) or it->parse_time == std::filesystem::file_time_type{} or
            it->parse_time != last_write_time) {
            // If the file is modified, reset the state of the module.
            it->state = module_t::state_type::out_of_date;
            modified = true;
        }

        if (it->state < new_state) {
            auto cursor = file_cursor(it->path);
            auto const only_prologue = new_state == module_t::state_type::prologue;

            ctx.e = {};
            if (auto r = parse_top(cursor, ctx, only_prologue)) {
                it->ast = std::move(r).value();
            } else {
                it->ast = nullptr;
            }

            it->upstream_paths = cursor.upstream_paths();
            it->errors = std::exchange(ctx.e, {});
            it->errors.print(it->upstream_paths);
        }

        it->parse_time = last_write_time;
        it->state = new_state;
        ++it;
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