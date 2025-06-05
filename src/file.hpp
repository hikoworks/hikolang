
#ifndef HL_FILE_HPP
#define HL_FILE_HPP

#include <filesystem>
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

    /** The file's content.
     *
     * @return The content of the file as a string view.
     */
    [[nodiscard]] std::string_view view() const;

    /** Get the line and column at the offset of the file.
     *
     * @param offset The offset in the file.
     * @return A pair containing the line number (0-based) and column number
     *        (0-based).
     */
    [[nodiscard]] std::pair<size_t, size_t> line_column(std::size_t offset) const;

private:
    std::size_t _id = 0;

    /** The absolute normalized path to the file.
     */
    std::filesystem::path _path;

    /** The offset where each line starts in the file.
     *
     * The first element is always 0, representing the start of the file.
     */
    std::vector<size_t> _line_offsets;

    /** Is this file a .hic source file.
     * 
     * This may be false when #line directives are used to track the source of
     * generated code.
     */
    bool _is_source_code = false;
};

[[nodiscard]] file& make_file(std::filesystem::path const& path);
[[nodiscard]] file& get_file(std::size_t file_id);

} // namespace hl

#endif // HL_FILE_HPP
