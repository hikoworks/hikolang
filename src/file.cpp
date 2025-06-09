
#include "file.hpp"
#include <vector>
#include <map>
#include <memory>
#include <cassert>
#include <limits>

namespace hl {

static std::vector<std::unique_ptr<file>> files_by_id;
static std::map<std::string, file*> files_by_name;

file::file(std::size_t id, std::filesystem::path const& path) : _id(id), _path(path)
{
    assert(not path.empty());
    assert(path.is_absolute());
    _path = std::filesystem::absolute(path);
    assert(_path.is_absolute());
    assert(_path.is_normalized());
    assert(_path.has_filename());
}

std::size_t file::id() const noexcept
{
    return _id;
}

std::filesystem::path const& file::path() const
{
    return _path;
}

[[nodiscard]] file& make_file(std::filesystem::path const& path)
{
    static std::size_t next_id = 1;
    return *new file(next_id++, path);
}

[[nodiscard]] file& get_file(std::size_t file_id)
{
    // In a real implementation, this would look up the file by its ID.
    // Here we just create a dummy file for demonstration purposes.
    return make_file(std::filesystem::path("dummy_file_" + std::to_string(file_id)));
}

file_handle::file_handle(std::size_t id)
{
    assert(id < files.size());
    _id = id;
}

file_handle::file_handle(std::string_view path)
{
    assert(not path.empty());


}
file_handle(std::filesystem::path const& path);

[[nodiscard]] file *file_handle::operator->() const
{
    assert(_id < files_by_id.size());
    return files_by_id[_id].get();
}

[[nodiscard]] file &operator*() const
{
    assert(_id < files_by_id.size());
    return *files_by_id[_id];
}

[[nodiscard]] std::size_t file::read(std::size_t position, std::span<char> buffer) const
{
    assert(position < _file_size);

    if (buffer.size() == 0) {
        return 0; // No data to read
    }

    auto const _ = std::scoped_lock(_mutex);

    if (not _file_stream.is_open()) {
        _file_stream.open(_path, std::ios::binary);
        if (not _file_stream) {
            throw std::runtime_error("Failed to open file stream: " + _path.string());
        }
    }

    _file_stream.seekg(position);
    if (not _file_stream) {
        throw std::runtime_error("Failed to seek in file stream: " + _path.string());
    }

    _file_stream.read(buffer.data(), buffer.size());
    if (not _file_stream) {
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

} // namespace hl