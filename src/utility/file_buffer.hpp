
#pragma once

#include "file.hpp"
#include <vector>
#include <map>

namespace hk {

/** A file object that is a memory buffer instead of a file on disk.
 */
class file_buffer : public file {
public:
    /** Create a file object with the given path.
     *
     * @param path The absolute normalized path to the file.
     */
    explicit file_buffer(std::filesystem::path path);

    /** Read the file content into a buffer.
     *
     * @param position The position in the file to start reading from.
     * @param buffer The buffer to read the content into.
     * @return The number of bytes read. If zero, the end of the file has been reached.
     */
    [[nodiscard]] std::size_t read(std::size_t position, std::span<char> buffer) override;

    std::size_t write(std::size_t position, std::span<char const> buffer) override;

private:
    static std::mutex _mutex;

    /** The file content stored in a vector.
     *
     * This is used to read the file content from memory instead of from disk.
     * It is initialized when the file is opened.
     */
    static std::map<std::filesystem::path, std::vector<char>> _content_by_path;

    std::filesystem::path _path;
};

/** Create a file buffer with the given content.
 * 
 * @param content The content to write to the file buffer.
 * @return A unique identifier for the file buffer.
 */
[[nodiscard]] std::filesystem::path make_file_buffer(std::span<char const> content);

/** Create a file buffer with the given content.
 * 
 * @param content The content to write to the file buffer.
 * @return A unique identifier for the file buffer.
 */
[[nodiscard]] std::filesystem::path make_file_buffer(std::string_view content);

[[nodiscard]] inline std::filesystem::path make_file_buffer(std::string_view content)
{
    return make_file_buffer(std::span<char const>{content.data(), content.size()});
}

[[nodiscard]] inline std::filesystem::path make_file_buffer(std::string const& content)
{
    return make_file_buffer(std::span<char const>{content.data(), content.size()});
}

[[nodiscard]] inline std::filesystem::path make_file_buffer(char const *content)
{
    auto view = std::string_view{content};
    return make_file_buffer(std::span<char const>{view.data(), view.size()});
}

}
