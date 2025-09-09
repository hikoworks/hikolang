
#include "repository.hpp"
#include "utility/path.hpp"
#include "utility/vector_set.hpp"
#include "utility/file_cursor.hpp"
#include "parser/parsers.hpp"
#include <cassert>
#include <algorithm>

namespace hk {

repository::repository(std::filesystem::path path) : _path(std::move(path))
{
    assert(std::filesystem::canonical(_path) == _path);
}

void repository::scan_prologues(bool force)
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

        auto &m = get_module(module_path);
        m.touched = true;

        if (m.ast and m.time == entry.last_write_time()) {
            // The prologue for this module has already been scanned.
            continue;
        }

        auto cursor = file_cursor(module_path);
        auto it = 

        m.errors.clear();
        if (auto r = parse_module(cursor, m.errors, true)) {

        }
    }

    untouch(true);
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

repository::module_type& repository::get_module(std::filesystem::path const& module_path)
{
    assert(is_subpath(module_path, _path));

    auto it = std::lower_bound(_modules.begin(), _modules.end(), module_path, [](auto const &item, auto const &key) {
        return item.path < key;
    });
    if (it == _modules.end() or it->path != module_path) {
        it = _modules.insert(it, module_type{module_path});
    }
    return *it;
}

repository::module_type::module_type(std::filesystem::path path) : path(std::move(path))
{
    assert(std::filesystem::canonical(this->path) == this->path);
}

}