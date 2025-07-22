
#include "file_buffer.hpp"
#include "file.hpp"
#include <cassert>
#include <format>
#include <mutex>
#include <map>

namespace hk {

std::mutex file_buffer::_mutex;
std::map<std::filesystem::path, std::vector<char>> file_buffer::_content_by_path;

file_buffer::file_buffer(std::filesystem::path path)
    : file(), _path(std::move(path))
{
}

[[nodiscard]] std::size_t file_buffer::read(std::size_t position, std::span<char> buffer)
{
    if (buffer.size() == 0) {
        return 0; // No data to read
    }

    auto const _ = std::scoped_lock(_mutex);
    auto it = _content_by_path.lower_bound(_path);
    if (it == _content_by_path.end() or it->first != _path) {
        it = _content_by_path.emplace_hint(it, _path, std::vector<char>{});
    }
    auto const &content = it->second;

    if (position >= content.size()) {
        return 0; // Position is beyond the end of the content
    }

    auto bytes_read = std::min(buffer.size(), content.size() - position);
    std::copy_n(content.data() + position, bytes_read, buffer.data());

    if (bytes_read < buffer.size()) {
        // If we read less than the buffer size, fill the rest with null characters.
        std::fill(buffer.begin() + bytes_read, buffer.end(), '\0');
    }

    return bytes_read;
}

std::size_t file_buffer::write(std::size_t position, std::span<char const> buffer)
{
    auto const _ = std::scoped_lock(_mutex);
    auto it = _content_by_path.lower_bound(_path);
    if (it == _content_by_path.end() or it->first != _path) {
        it = _content_by_path.emplace_hint(it, _path, std::vector<char>{});
    }
    auto &content = it->second;

    if (position >= content.size()) {
        content.resize(position + buffer.size(), '\0');
    }
    std::copy(buffer.begin(), buffer.end(), content.begin() + position);
    return buffer.size();
}

[[nodiscard]] static std::filesystem::path make_file_buffer_path()
{
    static std::atomic<uint64_t> counter = 0;

    // Generate a unique path for the file buffer.
    // This could be a temporary file or a specific directory for file buffers.
    return std::filesystem::path{std::format("/tmp/hic-buffer-{}.bin", ++counter)};
}

[[nodiscard]] std::filesystem::path make_file_buffer(std::span<char const> content)
{
    auto path = make_file_buffer_path();
    auto file = file_buffer{path};

    file.write(0, content);
    return path;
}

[[nodiscard]] std::filesystem::path make_file_buffer(std::string_view content)
{
    return make_file_buffer(std::span<char const>{content.data(), content.size()});
}

} // namespace hk
