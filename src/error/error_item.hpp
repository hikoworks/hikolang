#pragma once

#include "error_code.hpp"
#include "tokenizer/line_table.hpp"
#include <string>
#include <format>
#include <vector>
#include <cassert>
#include <filesystem>
#include <expected>

namespace hk {

class error_item {
public:
    constexpr error_item() noexcept = default;
    error_item(error_item const&) = default;
    error_item(error_item&&) = default;
    error_item& operator=(error_item const&) = default;
    error_item& operator=(error_item&&) = default;

    template<typename... Args>
    error_item(char const* first, char const* last, error_code code, std::format_string<Args...> fmt, Args&&... args) :
        _first(first), _last(last), _code(code), _message(std::format(std::move(fmt), std::forward<Args>(args)...))
    {
        assert(_code.has_value());
    }

    template<typename... Args>
    error_item(error_code code, std::format_string<Args...> fmt, Args&&... args) :
        _first(), _last(), _code(code), _message(std::format(std::move(fmt), std::forward<Args>(args)...))
    {
        assert(_code.has_value());
    }

    operator std::unexpected<error_code>() const
    {
        assert(_code.has_value());
        return std::unexpected{_code};
    }

    [[nodiscard]] constexpr char const* first() const noexcept
    {
        return _first;
    }

    [[nodiscard]] constexpr char const* last() const noexcept
    {
        return _last;
    }

    [[nodiscard]] constexpr error_code code() const noexcept
    {
        return _code;
    }

    [[nodiscard]] constexpr std::string_view message() const noexcept
    {
        return _message;
    }

    void print(line_table const& lines) const;

    /** Create a string to print to the console for the error.
     * 
     * @param text The text of the source-code.
     * @param upstream_paths Paths to the original source code.
     * @return A string to be send to the console.
     */
    [[nodiscard]] std::string to_string(line_table const& lines) const;

private:
    char const* _first = {};
    char const* _last = {};
    error_code _code = {};
    std::string _message = {};
};

} // namespace hk
