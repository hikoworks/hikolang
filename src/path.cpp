
#include "path.hpp"
#include <mutex>
#include <vector>
#include <map>
#include <filesystem>
#include <utility>
#include <stdexcept>

namespace hl {

inline static std::mutex _path_mutex;
inline static std::map<std::filesystem::path, path_id> _path_by_name;
inline static std::vector<decltype(_path_by_name)::node_type *> _path_by_id;

[[nodiscard]] path_id get_path_id(std::filesystem::path path)
{
    path = path.lexically_normal();

    if (path.empty()) {
        throw std::invalid_argument("Path cannot be empty.");
    }
    if (path.is_relative()) {
        throw std::invalid_argument("Path must be absolute.");
    }

    auto const _ = std::scoped_lock(_path_mutex);

    auto it = _path_by_name.lower_bound(path);
    if (it != _path_by_name.end() and it->first == path) {
        return it->second;
    }

    auto index = _path_by_id.size();
    it = _path_by_name.emplace_hint(it, std::move(path), static_cast<path_id>(index));
    _path_by_id.push_back(&*it);

    return static_cast<path_id>(index);
}

[[nodiscard]] path_id get_path_id(std::filesystem::path path, path_id relative_to)
{
    auto const &relative_to_path = get_path(relative_to);
    if (relative_to_path.has_filename()) {
        auto tmp = relative_to_path;
        tmp.remove_filename();
        assert(not tmp.empty());
        assert(tmp.is_absolute());

        return get_path_id(tmp / path);

    } else {
        assert(not relative_to_path.empty());
        assert(relative_to_path.is_absolute());
        return get_path_id(relative_to_path / path);
    }
}

[[nodiscard]] std::filesystem::path const &get_path(path_id id)
{
    auto const _ = std::scoped_lock(_path_mutex);
    auto node_ptr = _path_by_id.at(std::to_underlying(id));
    assert(node_ptr != nullptr);
    return node_ptr->first;
}

} // namespace hl
