
#pragma once

#include "file.hpp"
#include <fstream>

namespace hl {

/** A file object representing a file in the filesystem.
 * 
 * This class provides methods to read the content of a file, open and close it,
 * and manage its path identifier.
 * 
 * This class is thread-safe, meaning that multiple threads can access it
 * concurrently.
 */
class file_ifstream : public file {
public:
    /** Create a file object with the given path.
     *
     * @param path The absolute normalized path to the file.
     */
    explicit file_ifstream(hl::path_id path_id);

    /** Read the file content into a buffer.
     *
     * @param position The position in the file to start reading from.
     * @param buffer The buffer to read the content into.
     * @return The number of bytes read. If zero, the end of the file has been reached.
     */
    [[nodiscard]] std::size_t read(std::size_t position, std::span<char> buffer) override;

    /** Open the file.
     * 
     * This will open the file for reading. If the file is already open, this
     * will do nothing.
     */
    void open() override;

    /** Close the file.
     * 
     * This will close the file and release any resources associated with it.
     * If the file is not open, this will do nothing.
     */
    void close() noexcept override;

private:
    /** The file stream used to read the file.
     * 
     * This may be empty or closed if the file is not open.
     */
    std::ifstream _file_stream = {};

    /** Open the file without locking the mutex.
     */
    void _open();

    /** Close the file without locking the mutex.
     */
    void _close() noexcept;
};

} // namespace hl
