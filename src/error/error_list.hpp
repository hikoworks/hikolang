
#pragma once

#include "error_item.hpp"
#include "hkc_error.hpp"
#include "utility/fixed_string.hpp"
#include <vector>
#include <format>
#include <concepts>
#include <expected>
#include <algorithm>

namespace hk {

class error_list : public std::vector<error_item> {
public:
    using element_type = error_item;

    /** Add an error.
     *
     * This add will make sure the error_list is sorted by file order.
     * Parsing the file multiple times will not introduce duplicate errors.
     * 
     * @param lines The line table for this file. Used for print the error
     *              message to stderr.
     * @param first Pointer to the first character causing the error.
     * @param last Pointer to beyond the last character causing the error.
     * @param error The error code.
     * @param message An extra message to print.
     * @return The iterator to the error, true if a new error was inserted.
     */
    std::pair<iterator, bool> add(line_table const& lines, char const* first, char const* last, hkc_error error, std::string message = std::string{})
    {
        auto e = error_item{error, first, last, std::move(message)};

        auto it = std::lower_bound(begin(), end(), e, [](auto const& item, auto const& x) {
            return item < x;
        });
        if (it != end() and *it == e) {
            return {it, false};
        }

        it = insert(it, std::move(e));
        it->print(lines);
        return {it, true};
    }

    /** Add an error.
     *
     * This add will make sure the error_list is sorted by file order.
     * Parsing the file multiple times will not introduce duplicate errors.
     * 
     * @param lines The line table for this file. Used for print the error
     *              message to stderr.
     * @param first Pointer to the first character causing the error.
     * @param last Pointer to beyond the last character causing the error.
     * @param error The error code.
     * @param fmt Formatting string for an extra error message (optional)
     * @param args... Arguments for formatting.
     * @return The iterator to the error, true if a new error was inserted.
     */
    template<typename... Args>
    std::pair<iterator, bool> add(line_table const& lines, char const* first, char const* last, hkc_error code, std::format_string<Args...> fmt = {}, Args&&... args)
    {
        return add(lines, first, last, code, std::format(std::move(fmt), std::forward<Args>(args)...));
    }

    /** Add an error.
     *
     * This add will make sure the error_list is sorted by file order.
     * Parsing the file multiple times will not introduce duplicate errors.
     * 
     * @param lines The line table for this file. Used for print the error
     *              message to stderr.
     * @param first Pointer to the first character causing the error.
     * @param error The error code.
     * @param fmt Formatting string for an extra error message (optional)
     * @param args... Arguments for formatting.
     * @return The iterator to the error, true if a new error was inserted.
     */
    template<typename... Args>
    std::pair<iterator, bool> add(line_table const& lines, char const* first, hkc_error code, std::format_string<Args...> fmt = {}, Args&&... args)
    {
        return add(lines, first, nullptr, code, std::format(std::move(fmt), std::forward<Args>(args)...));
    }

    /** Add an error.
     *
     * This add will make sure the error_list is sorted by file order.
     * Parsing the file multiple times will not introduce duplicate errors.
     * 
     * @param lines The line table for this file. Used for print the error
     *              message to stderr.
     * @param error The error code.
     * @param fmt Formatting string for an extra error message (optional)
     * @param args... Arguments for formatting.
     * @return The iterator to the error, true if a new error was inserted.
     */
    template<typename... Args>
    std::pair<iterator, bool> add(line_table const& lines, hkc_error code, std::format_string<Args...> fmt = {}, Args&&... args)
    {
        return add(lines, nullptr, nullptr, code, std::format(std::move(fmt), std::forward<Args>(args)...));
    }

private:
};

} // namespace hk
