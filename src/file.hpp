
#ifndef HL_FILE_HPP
#define HL_FILE_HPP

#indlude <filesystem>
#include <cstddef>

namespace hl {

class file {
public:
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
    
private:
    std::size_t _id = 0;

    /** The absolute normalized path to the file.
     */
    std::filessystem::path _path;
};

[[nodiscard]] file& make_file(std::filesystem::path const& path);
[[nodiscard]] file& get_file(std::size_t file_id);

}

#endif // HL_FILE_HPP
