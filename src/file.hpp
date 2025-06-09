
#ifndef HL_FILE_HPP
#define HL_FILE_HPP

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
    class handle {
    public:
        constexpr handle() noexcept = default;
        handle(handle const&) noexcept = default;
        handle(handle&&) noexcept = default;
        handle& operator=(handle const&) noexcept = default;
        handle& operator=(handle&&) noexcept = default;
        [[nodiscard]] friend bool operator==(handle const&, handle const&) noexcept = default;

        [[nodiscard]] explicit operator bool() const noexcept
        {
            return _id != std::numeric_limits<std::size_t>::max();
        }

        [[nodiscard]] std::size_t id() const noexcept
        {
            return _id;
        }

        [[nodiscard]] file* operator->() const;
        
        [[nodiscard]] file& operator*() const;

    private:
        std::size_t _id = std::numeric_limits<std::size_t>::max();

        handle(std::size_t id) : _id(id) {}

        friend class file;
    };

    class reader {
    public:

        [[nodiscard]] char32_t operator[](std::size_t index) const
        {
            assert(_file != nullptr);
            assert(index < _lookahead.size());
            return _lookahead[index];
        }

    private:
        file* _file = nullptr;
        std::size_t _position = 0;
        std::array<char32_t, 8> _lookahead = {};
    };

    /** Create a file object with the given path.
     *
     * @param path The absolute normalized path to the file.
     */
    explicit file(std::size_t id, std::filesystem::path const& path);

    /** Unique identifier for the file
     */
    [[nodiscard]] std::size_t id() const noexcept;

    /** Get the absolute normalized path to the file.
     *
     * @return The absolute normalized path to the file.
     */
    [[nodiscard]] std::filesystem::path const& path() const;

    [[nodiscard]] std::size_t size() const noexcept
    {
        return _file_size;
    }

    /** Read the file content into a buffer.
     *
     * @param position The position in the file to start reading from.
     * @param buffer The buffer to read the content into.
     * @return The number of bytes read.
     */
    [[nodiscard]] std::size_t read(std::size_t position, std::span<char> buffer) const;

    void close() noexcept;

    [[nodiscard]] static handle make_file(std::string_view path);
    [[nodiscard]] static handle make_file(std::string_view path, handle relative_to);

private:
    inline static std::vector<std::unique_ptr<file>> _files_by_id;
    inline static std::map<std::string, file*> _files_by_name;

    std::size_t _id = 0;

    std::mutex _mutex;

    /** The size of the file in bytes.
     * 
     * This is set when the file is read for the first time.
     */
    std::size_t _file_size = 0;

    /** The absolute normalized path to the file.
     */
    std::filesystem::path _path;

    /** Is this file a .hic source file.
     * 
     * This may be false when #line directives are used to track the source of
     * generated code.
     */
    bool _is_source_code = false;

    std::ifstream _file_stream;
};



} // namespace hl

#endif // HL_FILE_HPP
