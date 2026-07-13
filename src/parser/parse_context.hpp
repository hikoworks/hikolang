
#pragma once

#include "error/error_list.hpp"
#include "tokenizer/line_table.hpp"
#include "ast/documentation_node.hpp"
#include "ast/attribute_node.hpp"
#include <filesystem>
#include <cassert>

namespace hk {

class parse_context {
public:
    parse_context(line_table lines) : _lines(std::move(lines)) {}

    error_list& errors()
    {
        return _errors;
    }

    line_table& lines()
    {
        return _lines;
    }

    std::unexpected<hkc_error> add(char const* first, char const* last, hkc_error error, std::string message = std::string{})
    {
        auto const [it, _] = errors().add(lines(), first, last, error, std::move(message));
        return std::unexpected{it->code()};
    }

    std::unexpected<hkc_error> add(char const* first, hkc_error error, std::string message = std::string{})
    {
        auto const [it, _] = errors().add(lines(), first, nullptr, error, std::move(message));
        return std::unexpected{it->code()};
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
    std::unexpected<hkc_error> add(char const* first, char const* last, hkc_error error, std::format_string<Args...> fmt, Args&&... args)
    {
        return add(first, last, error, std::format(std::move(fmt), std::forward<Args>(args)...));
    }

    /** Add an error to the list.
     *
     *  @tparam ErrorCode The error being raised, including the format string.
     *  @param first The first file location where the error occurred.
     *  @param args The arguments to format the error message.
     *  @return A unexpected error containing the error code.
     */
    template<typename... Args>
    std::unexpected<hkc_error> add(char const* first, hkc_error error, std::format_string<Args...> fmt, Args&&... args)
    {
        return add(first, nullptr, error, std::format(std::move(fmt), std::forward<Args>(args)...));
    }

    /** Add an error to the list.
     *
     *  @tparam ErrorCode The error being raised, including the format string.
     *  @param args The arguments to format the error message.
     *  @return A unexpected error containing the error code.
     */
    template<typename... Args>
    std::unexpected<hkc_error> add(hkc_error error, std::format_string<Args...> fmt, Args&&... args)
    {
        return add(nullptr, nullptr, error, std::format(std::move(fmt), std::forward<Args>(args)...));
    }

private:
    std::unique_ptr<ast::documentation_node> _documentation;
    std::vector<std::unique_ptr<ast::attribute_node>> _attributes;
    error_list _errors;
    line_table _lines;
};


}

