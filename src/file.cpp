
#include "file.hpp"
#include <vector>

namespace hl {

static std::vector<file> files;

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

} // namespace hl