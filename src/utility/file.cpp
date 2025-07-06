
#include "file.hpp"
#include "file_ifstream.hpp"
#include "file_buffer.hpp"
#include <map>
#include <memory>
#include <cassert>

namespace hk {

file::file(hk::path_id path_id)
    : _path_id(path_id)
{
}


inline static std::mutex file_mutex;
inline static std::map<path_id, std::unique_ptr<file>> file_by_path_id;

[[nodiscard]] file& get_file(hk::path_id path_id)
{
    auto const _ = std::scoped_lock(file_mutex);

    auto it = file_by_path_id.lower_bound(path_id);
    if (it != file_by_path_id.end() and it->first == path_id) {
        return *(it->second);
    }

    auto file_ptr = std::unique_ptr<file>{};

    auto const& path = get_path(path_id);
    auto rel_path = path.relative_path();
    auto rel_path_str = rel_path.generic_string();

    if (rel_path_str.starts_with("tmp/hic-buffer-") and rel_path_str.ends_with(".bin")) {
        file_ptr = std::make_unique<file_buffer>(path_id);
    } else {
        file_ptr = std::make_unique<file_ifstream>(path_id);
    }
    assert(file_ptr != nullptr);

    it = file_by_path_id.emplace_hint(it, path_id, std::move(file_ptr));
    return *(it->second);
}   

} // namespace hk