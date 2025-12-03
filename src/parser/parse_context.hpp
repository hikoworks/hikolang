
#pragma once

#include "error/error_list.hpp"
#include "tokenizer/line_table.hpp"
#include <filesystem>
#include <cassert>

namespace hk {

class parse_context {
public:
    parse_context(error_list &errors, line_table &lines) : _errors(std::addressof(errors)), _lines(std::addressof(lines)) {}

    error_list& errors()
    {
        assert(_errors != nullptr);
        return *_errors;
    }

    line_table& lines()
    {
        assert(_lines != nullptr);
        return *_lines;
    }

    /** Add an error to the list.
     *
     *  @tparam ErrorCode The error being raised, including the format string.
     *  @param first The first file location where the error occurred.
     *  @param last The last file location where the error occurred.
     *  @param args The arguments to format the error message.
     *  @return A unexpected error containing the error code.
     */
    template<typename... Args>
    std::unexpected<hkc_error> add(char const* first, char const* last, hkc_error error, std::format_string<Args...> msg, Args&&... args)
    {
        return std::unexpected{errors().add(first, last, error, std::move(msg), std::forward<Args>(args)...)};
    }

    /** Add an error to the list.
     *
     *  @tparam ErrorCode The error being raised, including the format string.
     *  @param first The first file location where the error occurred.
     *  @param args The arguments to format the error message.
     *  @return A unexpected error containing the error code.
     */
    template<typename... Args>
    std::unexpected<hkc_error> add(char const* first, hkc_error error, std::format_string<Args...> msg, Args&&... args)
    {
        return std::unexpected{errors().add(first, error, std::move(msg), std::forward<Args>(args)...)};
    }

    /** Add an error to the list.
     *
     *  @tparam ErrorCode The error being raised, including the format string.
     *  @param args The arguments to format the error message.
     *  @return A unexpected error containing the error code.
     */
    template<typename... Args>
    std::unexpected<hkc_error> add(hkc_error error, std::format_string<Args...> msg, Args&&... args)
    {
        return std::unexpected{errors().add(error, std::move(msg), std::forward<Args>(args)...)};
    }

    /** Add an error to the list.
     *
     *  @tparam ErrorCode The error being raised, including the format string.
     *  @param first The first file location where the error occurred.
     *  @param last The last file location where the error occurred.
     *  @param args The arguments to format the error message.
     *  @return A unexpected error containing the error code.
     */
    std::unexpected<hkc_error> add(char const* first, char const* last, hkc_error error)
    {
        return std::unexpected{errors().add(first, last, error)};
    }

    /** Add an error to the list.
     *
     *  @tparam ErrorCode The error being raised, including the format string.
     *  @param first The first file location where the error occurred.
     *  @param args The arguments to format the error message.
     *  @return A unexpected error containing the error code.
     */
    std::unexpected<hkc_error> add(char const* first, hkc_error error)
    {
        return std::unexpected{errors().add(first, error)};
    }

    /** Add an error to the list.
     *
     *  @tparam ErrorCode The error being raised, including the format string.
     *  @param args The arguments to format the error message.
     *  @return A unexpected error containing the error code.
     */
    std::unexpected<hkc_error> add(hkc_error error)
    {
        return std::unexpected{errors().add(error)};
    }

    

private:
    error_list *_errors = nullptr;
    line_table *_lines = nullptr;
};


}

