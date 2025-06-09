
#ifndef HL_FILE_HPP
#define HL_FILE_HPP

#include "path.hpp"
#include <filesystem>
#include <cstddef>
#include <string>
#include <string_view>
#include <limits>
#include <memory>
#include <cassert>
#include <vector>
#include <map>
#include <array>
#include <span>
#include <mutex>

namespace hl {

class file {
public:
    /** Create a file object with the given path.
     *
     * @param path The absolute normalized path to the file.
     */
    explicit file(hl::path_id path_id);

    /** Get the absolute normalized path to the file.
     *
     * @return The absolute normalized path to the file.
     */
    [[nodiscard]] std::filesystem::path const& path() const
    {
        return hl::get_path(_path_id);
    }

    /** Read the file content into a buffer.
     *
     * @param position The position in the file to start reading from.
     * @param buffer The buffer to read the content into.
     * @return The number of bytes read. If zero, the end of the file has been reached.
     */
    [[nodiscard]] std::size_t read(std::size_t position, std::span<char> buffer) const;

    void close() noexcept;

private:
    std::mutex _mutex;

    /** The absolute normalized path to the file.
     */
    path_id _path_id = path_id::invalid;

    /** Is this file a .hic source file.
     * 
     * This may be false when #line directives are used to track the source of
     * generated code.
     */
    bool _is_source_code = false;

    std::ifstream _file_stream = {};
};

/** Get a file object pointing the the given path identifier.
 * 
 * @param path_id The path identifier to get the file for.
 * @return A file object pointing to the file, or an error code if the file could not be opened.
 */
[[nodiscard]] file &get_file(hl::path_id path_id);

/** Remove the file object from memory.
 * 
 * This will close the file and remove it from the internal cache.
 * This is used when the file no longer exists during incremental compilation.
 * 
 * @param path_id The path identifier of the file object to remove.
 */
void free_file(hl::path_id path_id) noexcept;

} // namespace hl

#endif // HL_FILE_HPP
