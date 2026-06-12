
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
    constexpr line_table() = default;

    /** Clear the line table.
     */
    void clear();

    /** Get the source position.
     * 
     * @note It is UB if @a p is not part of the text
     *       described by the line_table.
     * @note It is UB if @a p is pointing inside a UTF-8 code-point sequence.
     * @note It is UB if @a p is pointing in text that is not terminated with nul.  
     * @param p The character pointer for which the position is needed.
     * @return source file name, line number, utf-16 column number.
     */
    [[nodiscard]] std::tuple<interned_string, size_t, size_t> get_position(char const *p) const; 

    [[nodiscard]] std::string_view get_line_text(char const *p) const;

    /** Start a file.
     * 
     * @param p Pointer to the first character in the actual source file.
     * @param path The file-name of the file.
     */
    void add_sof(char const *p, std::string_view path);

    /** End a file.
     * 
     * @param p Pointer one-past the actual source file.
     */
    void add_eof(char const *p);

    void add_line(char const *p, size_t line);

    void add_line(char const *p, size_t line, std::string_view path);


private:
    struct sync_point {
        /** The path to the file.
         */
        interned_string path;

        /** Pointer to the first character on a line.
         */
        char const *p;

        /** Flags
         * 
         * 0: start of file
         * 1: end of file
         * 2: line sync
         */
        size_t flag : 2;

        /** Line number (0-based) of the line.
         */
        size_t line : 62;
    };

    /** The list of line synchronization points.
     */
    std::vector<sync_point> _sync_points;
};

}
