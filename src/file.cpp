
#include "file.hpp"
#include <vector>
#include <map>
#include <memory>
#include <cassert>
#include <limits>

namespace hl {

file::file(hl::path_id path_id)
    : _path_id(path_id)
{
}

[[nodiscard]] std::size_t file::read(std::size_t position, std::span<char> buffer) const
{
    assert(position < _file_size);

    if (buffer.size() == 0) {
        return 0; // No data to read
    }

    auto const &path = get_path(_path_id);

    auto const _ = std::scoped_lock(_mutex);

    if (not _file_stream.is_open()) {
        _file_stream.open(_path, std::ios::binary);
        if (not _file_stream) {
            throw std::runtime_error("Failed to open file stream: " + _path.string());
        }
    }

    if (not _file_stream.seekg(position)) {
        // Could not seek to the position, possibly because the position is
        // beyond the end of the file.
        close();
        return 0;
    }

    _file_stream.read(buffer.data(), buffer.size());
    if (not _file_stream) {
        close();
        throw std::runtime_error("Failed to read from file stream: " + _path.string());
    }

    return _file_stream.gcount();
}

void file::close() noexcept
{
    auto const _ = std::scoped_lock(_mutex);
    if (_file_stream.is_open()) {
        _file_stream.close();
    }
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

    it = file_by_path_id.emplace_hint(it, path_id, std::make_unique<file>(path_id));
    return *(it->second);
}

} // namespace hl