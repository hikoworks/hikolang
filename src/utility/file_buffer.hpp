
#pragma once

#include "file.hpp"
#include <vector>

namespace hl {

class file_buffer : public file {
public:
    /** Create a file object with the given path.
     *
     * @param path The absolute normalized path to the file.
     */
    explicit file_buffer(hl::path_id path_id);

    /** Read the file content into a buffer.
     *
     * @param position The position in the file to start reading from.
     * @param buffer The buffer to read the content into.
     * @return The number of bytes read. If zero, the end of the file has been reached.
     */
    [[nodiscard]] std::size_t read(std::size_t position, std::span<char> buffer) override;

    [[nodiscard]] std::size_t write(std::size_t position, std::span<char const> buffer) override;

private:
    /** The file content stored in a vector.
     *
     * This is used to read the file content from memory instead of from disk.
     * It is initialized when the file is opened.
     */
    std::vector<char> _content = {};
};

/** Create a file buffer with the given content.
 * 
 * @param content The content to write to the file buffer.
 * @return A unique identifier for the file buffer.
 */
[[nodiscard]] path_id make_file_buffer(std::span<char const> content);

/** Create a file buffer with the given content.
 * 
 * @param content The content to write to the file buffer.
 * @return A unique identifier for the file buffer.
 */
[[nodiscard]] path_id make_file_buffer(std::string_view content);

}
