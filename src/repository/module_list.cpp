
#include "module_list.hpp"
#include <algorithm>

namespace hk {

std::pair<module_list::iterator, bool> module_list::insert(std::filesystem::path const& path)
{
    assert(not empty() or _state == state_type::ordered_by_path);

    if (_state == state_type::ordered_by_path) {
        auto it = std::lower_bound(begin(), end(), path, [](auto const& item, auto const& x) {
            return item.path < x;
        });

        if (it == end() or it->path != path) {
            return {_modules.insert(it, value_type{path}), true};
        } else {
            return {it, false};
        }

    } else {
        auto it = std::find_if(begin(), end(), [&path](auto const& item) {
            return item.path == path;
        });

        if (it == end()) {
            _state = state_type::unordered;
            _modules.emplace_back(path);
            return {_modules.end() - 1, true};

        } else {
            return {it, false};
        }
    }
}

bool module_list::erase(std::filesystem::path const& path)
{
    assert(not empty() or _state == state_type::ordered_by_path);
    
    if (_state == state_type::ordered_by_path) {
        auto it = std::lower_bound(begin(), end(), path, [](auto const& item, auto const& x) {
            return item.path < x;
        });

        if (it == end() or it->path != path) {
            return false;
        }

        _modules.erase(it);
        return true;

    } else {
        auto it = std::find_if(begin(), end(), [&path](auto const& item) {
            return item.path == path;
        });

        if (it == end()) {
            return false;
        }

        _modules.erase(it);
        _state = empty() ? state_type::ordered_by_path : state_type::unordered;

        return true;
    }
}

module_list::iterator module_list::find(std::filesystem::path const& path)
{
    if (_state == state_type::ordered_by_path) {
        auto it = std::lower_bound(begin(), end(), path, [](auto const& item, auto const& value) {
            return item.path < value;
        });

        if (it != end() and it->path != path) {
            it = end();
        }

        return it;

    } else {
        return std::find_if(begin(), end(), [&path](auto const& item) {
            return item.path == path;
        });
    }
}

void module_list::sort_by_path()
{
    _state = state_type::ordered_by_path;
    std::sort(begin(), end(), [](auto const& a, auto const& b) {
        return a.path < b.path;
    });
}

void module_list::sort_by_compilation()
{
    _state = state_type::compilation_order;
}

}
