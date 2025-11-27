
#pragma once

#include <string>
#include <vector>
#include <tuple>

namespace hk {

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
    [[nodiscard]] std::tuple<std::string, size_t, size_t> get_position(char const *p) const; 

    /** Set the line number.
     *
     * @note It is UB is `add(p, lineno, file_name)` has not been called
     *       before.
     * @param p The character pointer to where the line starts.
     * @param lineno The line number of the current file. 0-based.
     */
    void add(char const* p, size_t lineno);

    /** Set the line number and file_name.
     * 
     * @param p The character pointer to where the line starts.
     * @param lineno The line number of the current file. 0-based.
     * @param file_name The file name of the current file.
     */
    void add(char const* p, size_t lineno, std::string_view file_name);

private:
    struct sync_point {
        /** Pointer to the first character on a line.
         */
        char const *p;

        /** Line number (0-based) of the line.
         */
        size_t lineno;

        /** The file where the line is located.
         */
        size_t fileno;
    };

    /** The list of source files.
     */
    std::vector<std::string> _file_names;

    /** The list of line synchronization points.
     */
    std::vector<sync_point> _sync_points;

    [[nodiscard]] size_t get_fileno(std::string_view file_name);
};

}
