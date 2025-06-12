
#include "file_vector.hpp"

namespace hl {

file_vector::file_vector(hl::path_id path_id)
    : file(path_id)
{
}

[[nodiscard]] std::size_t file_vector::read(std::size_t position, std::span<char> buffer)
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

std::size_t file_vector::write(std::size_t position, std::span<char const> buffer)
{
    if (position >= _content.size()) {
        _content.resize(position + buffer.size(), '\0');
    }
    std::copy(buffer.begin(), buffer.end(), _content.begin() + position);
    return buffer.size();
}

} // namespace hl
