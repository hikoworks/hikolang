
#include "file.hpp"
#include "file_ifstream.hpp"
#include <map>
#include <memory>


namespace hl {

file::file(hl::path_id path_id)
    : _path_id(path_id)
{
}


inline static std::mutex file_mutex;
inline static std::map<path_id, std::unique_ptr<file>> file_by_path_id;

[[nodiscard]] file& get_file(hl::path_id path_id)
{
    auto const _ = std::scoped_lock(file_mutex);

    auto it = file_by_path_id.lower_bound(path_id);
    if (it != file_by_path_id.end() and it->first == path_id) {
        return *(it->second);
    }

    it = file_by_path_id.emplace_hint(it, path_id, std::make_unique<file_ifstream>(path_id));
    return *(it->second);
}

} // namespace hl