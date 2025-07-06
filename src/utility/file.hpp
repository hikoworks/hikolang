
#pragma once

#include "path.hpp"
#include <span>
#include <mutex>
#include <cstddef>

namespace hk {

/** A file object representing a file in the filesystem.
 * 
 * This class provides methods to read the content of a file, open and close it,
 * and manage its path identifier.
 * 
 * This class is thread-safe, meaning that multiple threads can access it
 * concurrently.
 */
class file {
public:
    virtual ~file() = default;

    /** Create a file object with the given path.
     *
     * @param path The absolute normalized path to the file.
     */
    explicit file(hk::path_id path_id);

    /** Read the file content into a buffer.
     *
     * @param position The position in the file to start reading from.
     * @param buffer The buffer to read the content into.
     * @return The number of bytes read. If zero, the end of the file has been reached.
     */
    [[nodiscard]] virtual std::size_t read(std::size_t position, std::span<char> buffer)
    {
        throw std::runtime_error{"Read operation not supported on this file type."};
    }

    /** Write the file content from a buffer.
     *
     * @param position The position in the file to start writing to.
     * @param buffer The buffer to write the content from.
     * @return The number of bytes written.
     */
    virtual std::size_t write(std::size_t position, std::span<char const> buffer)
    {
        throw std::runtime_error{"Write operation not supported on this file type."};
    }

    /** Open the file.
     * 
     * This will open the file for reading. If the file is already open, this
     * will do nothing.
     */
    virtual void open() {};

    /** Close the file.
     * 
     * This will close the file and release any resources associated with it.
     * If the file is not open, this will do nothing.
     */
    virtual void close() noexcept {};

protected:
    /** This class is thread-safe, so we need a mutex to protect access to the file stream.
     */
    std::mutex _mutex;

    /** The absolute normalized path to the file.
     */
    path_id _path_id = path_id::invalid;
};

/** Get a file object pointing the the given path identifier.
 * 
 * @param path_id The path identifier to get the file for.
 * @return A file object pointing to the file, or an error code if the file could not be opened.
 */
[[nodiscard]] file &get_file(hk::path_id path_id);

/** Remove the file object from memory.
 * 
 * This will close the file and remove it from the internal cache.
 * This is used when the file no longer exists during incremental compilation.
 * 
 * @param path_id The path identifier of the file object to remove.
 */
void free_file(hk::path_id path_id) noexcept;

} // namespace hk
