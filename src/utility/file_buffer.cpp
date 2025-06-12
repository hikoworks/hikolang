
#include "file_buffer.hpp"
#include "file.hpp"
#include <cassert>

namespace hl {

file_buffer::file_buffer(hl::path_id path_id)
    : file(path_id)
{
}

[[nodiscard]] std::size_t file_buffer::read(std::size_t position, std::span<char> buffer)
{
    if (buffer.size() == 0) {
        return 0; // No data to read
    }

    auto const _ = std::scoped_lock(_mutex);

    if (position >= _content.size()) {
        return 0; // Position is beyond the end of the content
    }

    auto bytes_read = std::min(buffer.size(), _content.size() - position);
    std::copy_n(_content.data() + position, bytes_read, buffer.data());

    if (bytes_read < buffer.size()) {
        // If we read less than the buffer size, fill the rest with null characters.
        std::fill(buffer.begin() + bytes_read, buffer.end(), '\0');
    }

    return bytes_read;
}

std::size_t file_buffer::write(std::size_t position, std::span<char const> buffer)
{
    if (position >= _content.size()) {
        _content.resize(position + buffer.size(), '\0');
    }
    std::copy(buffer.begin(), buffer.end(), _content.begin() + position);
    return buffer.size();
}

[[nodiscard]] path_id make_file_buffer(std::span<char const> content)
{
    auto path = get_path_id();
    auto& file = get_file(path);
    assert(dynamic_cast<file_buffer*>(&file) != nullptr);

    file.write(0, content);
    return path;
}

[[nodiscard]] path_id make_file_buffer(std::string_view content)
{
    return make_file_buffer(std::span<char const>{content.data(), content.size()});
}

} // namespace hl
