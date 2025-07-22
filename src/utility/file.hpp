
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
 */
class file {
public:
    virtual ~file() = default;

    file() = default;

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
};

/** Make a file object from a path.
 * 
 * @param path The path to open.
 */
[[nodiscard]] std::unique_ptr<file> make_file(std::filesystem::path path);

} // namespace hk
