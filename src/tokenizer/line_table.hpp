
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
     * @note It is UB if @a p is pointing inside a UTF-8 code-point sequence.
     * @note It is UB if @a p is pointing in text that is not terminated with nul.  
     * @param p The character pointer for which the position is needed.
     * @return source file name, line number, utf-16 column number, the text of the line where the character is located.
     */
    [[nodiscard]] std::tuple<interned_string, uint32_t, uint32_t, std::string_view> get_position(char const *p) const; 

    void add(char const* p, std::string_view path, uint32_t line, sync_type kind);

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

        /** Flags
         */
        sync_type kind = sync_type::eof;

        /** Line number (0-based) of the line.
         */
        uint32_t line = 0;
    };

    /** The list of line synchronization points.
     */
    std::vector<sync_point_type> _sync_points = {};
};

}
