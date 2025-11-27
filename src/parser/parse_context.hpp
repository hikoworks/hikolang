
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
    template<std::derived_from<error_code_and_message_base> Message, typename... Args>
    std::unexpected<error_code> add_error(char const* first, char const* last, Message msg, Args&&... args)
    {
        return errors().add(first, last, std::move(msg), std::forward<Args>(args)...);
    }

    /** Add an error to the list.
     *
     *  @tparam ErrorCode The error being raised, including the format string.
     *  @param args The arguments to format the error message.
     *  @return A unexpected error containing the error code.
     */
    template<std::derived_from<error_code_and_message_base> Message, typename... Args>
    std::unexpected<error_code> add_error(Message msg, Args&&... args)
    {
        return errors().add(std::move(msg), std::forward<Args>(args)...);
    }

private:
    error_list *_errors = nullptr;
    line_table *_lines = nullptr;
};


}

