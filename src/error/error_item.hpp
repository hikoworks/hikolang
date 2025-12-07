#pragma once

#include "hkc_error.hpp"
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

    error_item(hkc_error code, char const* first = nullptr, char const* last = nullptr, std::string message = std::string{}) :
        _first(first), _last(last), _code(code), _message(std::move(message))
    {
    }

    operator std::unexpected<hkc_error>() const
    {
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

    [[nodiscard]] constexpr hkc_error code() const noexcept
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
    hkc_error _code = {};
    std::string _message = {};
};

} // namespace hk
