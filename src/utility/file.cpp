
#include "file.hpp"
#include "file_ifstream.hpp"
#include "file_vector.hpp"
#include <map>
#include <memory>
#include <cassert>

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

    auto file_ptr = std::unique_ptr<file>{};

    auto const& path = get_path(path_id);
    auto rel_path = path.relative_path();
    auto rel_path_str = rel_path.generic_string();

    if (rel_path_str.starts_with("file_vector/")) {
        file_ptr = std::make_unique<file_vector>(path_id);
    } else {
        file_ptr = std::make_unique<file_ifstream>(path_id);
    }
    assert(file_ptr != nullptr);

    it = file_by_path_id.emplace_hint(it, path_id, std::move(file_ptr));
    return *(it->second);
}

static std::atomic<uint64_t> file_vector_id = 0;

[[nodiscard]] file& get_file_vector()
{
    auto id = file_vector_id.fetch_add(1, std::memory_order_relaxed);
    auto path_name = std::filesystem::path{std::format("/file_vector/{}.bin", id)};
    auto path_id = hl::get_path_id(path_name);
    return hl::get_file(path_id);
}

[[nodiscard]] file& get_file_vector(std::string_view content)
{
    auto& f = get_file_vector();
    f.write(0, std::span<char const>{content.data(), content.size()});
    return f;
}
    

} // namespace hl