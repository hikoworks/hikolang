
#pragma once

#include "tokenizer/line_table.hpp"
#include <gsl/gsl>

namespace hk {

/** This class holds an error-location in module.
 *
 * If a error occurs outside of the AST tree, this class can be used to report
 * an error inside that module.
 */
class error_location {
public:
    constexpr error_location(error_location const&) noexcept = default;
    constexpr error_location(error_location&&) noexcept = default;
    constexpr error_location& operator=(error_location const&) noexcept = default;
    constexpr error_location& operator=(error_location&&) noexcept = default;

    /** Create an error location.
     *
     * @param errors The error list of the module.
     * @param first The location of the first character of the module where an
     *              optional error can appear.
     * @param last One beyond the last character.
     */
    constexpr error_location(line_table& lines, char const* first, char const* last = nullptr) :
        _lines(gsl::make_not_null(&lines)), _first(first), _last(last)
    {
    }

    template<typename... Args>
    hkc_error add(hkc_error error, std::format_string<Args...>, Args&&... args)
    {

    }

private:
    gsl::not_null<line_table *> _lines;
    char const* _first = nullptr;
    char const* _last = nullptr;
};

} // namespace hk