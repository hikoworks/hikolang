
#include "repository.hpp"
#include "utility/path.hpp"
#include "utility/vector_set.hpp"
#include "utility/git.hpp"
#include "parser/parse_top.hpp"
#include <cassert>
#include <algorithm>
#include <map>
#include <set>
#include <print>

namespace hk {

[[nodiscard]] static std::vector<source*> sort_by_name(std::vector<std::unique_ptr<source>> const& sources)
{
    auto sources_by_name = std::vector<source*>{};
    sources_by_name.reserve(sources.size());
    for (auto& source : sources) {
        sources_by_name.push_back(source.get());
    }

    std::sort(sources_by_name.begin(), sources_by_name.end(), [](auto const& a, auto const& b) {
        return cmp_names(*a, *b) == std::strong_ordering::less;
    });

    return sources_by_name;
}

repository::repository(std::filesystem::path path, repository_url remote) : remote(remote), path(std::move(path)) {}

void repository::scan_prologues(datum_namespace const& guard_namespace, module_list& modules)
{
    gather_modules();
    parse_prologues();
    _sources_by_name = sort_by_name(_sources_by_path);
    evaluate_build_guard(guard_namespace);
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

std::expected<void, hkc_error> repository::evaluate_build_guard(datum_namespace const& ctx)
{
    auto last_error = hkc_error::none;
    for (auto &source : _sources_by_path) {
        if (auto r = source->evaluate_build_guard(ctx); not r.has_value()) {
            last_error = r.error();
        }
    }
    
    if (last_error != hkc_error::none) {
        return std::unexpected{last_error};
    }

    return {};
}

bool repository::parse_prologues()
{
    auto modified = false;

    for (auto& source : _sources_by_path) {
        if (source->is_generated()) {
            // prologues of generated files are handled during compilation.
            continue;
        }

        if (auto r = source->parse_prologue(); not r) {
            std::println(stderr, "Could not get prologue of file '{}': {}", source->path().string(), r.error().message());
            continue;
        }

        modified = true;
    }

    return modified;
}

void repository::recursive_scan_prologues(datum_namespace const& guard_namespace, repository_flags flags)
{
    struct all_nodes_item {
        std::set<ast::node*> nodes;
        std::set<hkc_error> errors;

        all_nodes_item() = default;
        all_nodes_item(ast::node* node) : nodes{node}, errors{} {}
    };

    auto all_nodes = std::map<repository_url, all_nodes_item>{};
    auto todo = std::set<repository_url>{};

    auto modules = module_list{};

    scan_prologues(guard_namespace, modules);
    for (auto const node_ptr : remote_repositories()) {
        auto [it, inserted] = all_nodes.emplace(node_ptr->url, all_nodes_item{});
        it->second.nodes.insert(node_ptr);
        todo.emplace(node_ptr->url);
    }

    auto const hkdeps_path = path / "_hkdeps";
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
        auto child_remote_n = todo.extract(todo.begin());
        auto const child_remote = std::move(child_remote_n).value();

        auto const child_local_path = hkdeps_path / child_remote.directory();
        auto& child_repo = get_child_repository(child_remote, child_local_path);
        if (child_repo.mark) {
            continue;
        }

        child_repo.mark = true;
        if (auto r = git_checkout_or_clone(child_remote, child_local_path, flags); r != git_error::ok) {
            auto short_hkdeps = std::format("_hkdeps/{}", child_remote.directory());

            auto it = all_nodes.find(child_remote);
            assert(it != all_nodes.end());
            it->second.errors.insert(hkc_error::could_not_clone_repository);

            erase_child_repository(child_remote);
            continue;
        }

        child_repo.scan_prologues(guard_namespace, modules);
        for (auto node_ptr : child_repo.remote_repositories()) {
            all_nodes.emplace(node_ptr->url, node_ptr);
            todo.emplace(node_ptr->url);
        }
    }

    // Remove internal repositories not in 'done'.
    std::erase_if(_child_repositories, [&](auto const& item) {
        return not item->mark;
    });

    // Add error to each import statement that tried to import the repository.
    for (auto const& [url, item] : all_nodes) {
        for (auto error : item.errors) {
            for (auto node_ptr : item.nodes) {
                node_ptr->add(error, "url: {}, rev:, dir: _hkdep/{}", url.url(), url.rev(), url.directory());
            }
        }
    }
}

[[nodiscard]] generator<ast::import_repository_declaration_node*> repository::remote_repositories() const
{
    for (auto const& source: _sources_by_path) {
        assert(source);
        if (source->enabled()) {
            for (auto const& remote : source->remote_repositories()) {
                co_yield remote;
            }
        }
    }
}

[[nodiscard]] generator<source*> repository::get_anchors() const
{
    co_return;
}

[[nodiscard]] generator<source*> repository::get_modules_by_anchor(fqname anchor) const
{
    co_return;
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