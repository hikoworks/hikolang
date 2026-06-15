
#pragma once

#include "utility/interned_string.hpp"
#include <string>
#include <vector>
#include <tuple>

namespace hk {

/** A table that links a character pointer to a location in a source file.
 * 
 */
class line_table {
public:
    enum class sync_type {
        eof,
        sof,
        sol
    };

    constexpr line_table() = default;

    /** Clear the line table.
     */
    void clear();

    /** Get the source position.
     * 
     * @note It is UB if @a p is not part of the text
     *       described by the line_table.
     * @param p The character pointer for which the position is needed.
     * @return source file name, line number, utf-16 column number, the text of the line where the character is located.
     */
    [[nodiscard]] std::tuple<interned_string, uint32_t, uint32_t, std::string_view> get_position(char const *p) const;

    /** Get the location of an error.
     * 
     * The textual location are two lines of text, showing graphically where
     * the error is located.
     * 
     * @note It is UB if @a first or @a last are not part of the text
     *       described by the line_table.
     * @param first Pointer to the first character of a the first token.
     * @param last Pointer beyond the last character of the last token.
     * @return path to the source, line number, utf-16 column number, textual location.
     */
    [[nodiscard]] std::tuple<interned_string, uint32_t, uint32_t, std::string> get_error_location(
        char const *first, char const *last=nullptr) const;

    void add(char const* p, std::string_view path, uint32_t line, sync_type kind);

    /** Start a file.
     * 
     * @param p Pointer to the first character in the actual source file.
     * @param path The file-name of the file.
     */
    void add_file(char const *begin, char const *end, std::string_view path);

    void add_sol(char const *p, uint32_t line);

    void add_sol(char const *p, std::string_view path, uint32_t line);


private:
    struct sync_point_type {
        /** Pointer to the first character on a line.
         */
        char const *p = nullptr;

        /** The path to the file.
         */
        interned_string path = {};

        /** Line number (0-based) of the line.
         */
        uint32_t line = 0;

        /** Flags
         */
        sync_type kind = sync_type::eof;
    };

    /** The list of line synchronization points.
     */
    std::vector<sync_point_type> _sync_points = {};
};

}
