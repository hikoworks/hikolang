#pragma once

#include "error_code.hpp"
#include "tokenizer/file_location.hpp"
#include <string>
#include <format>
#include <vector>
#include <cassert>

namespace hk {

class error_cause {
public:

private:
    file_location _first;
    file_location _last;
    std::string _message;
};

class error_item {
public:
    constexpr error_item() noexcept = default;
    error_item(error_item const&) = default;
    error_item(error_item&&) = default;
    error_item& operator=(error_item const&) = default;
    error_item& operator=(error_item&&) = default;

    template<typename... Args>
    error_item(file_location first, file_location last, error_code code, std::format_string<Args...> fmt, Args&&... args)
        : _first(first), _last(last), _code(code), _message(std::format(std::move(fmt), std::forward<Args>(args)...))
    {
        assert(_code.has_value());
    }

    template<typename... Args>
    error_item(error_code code, std::format_string<Args...> fmt, Args&&... args)
        : _first(), _last(), _code(code), _message(std::format(std::move(fmt), std::forward<Args>(args)...))
    {
        assert(_code.has_value());
    }

    template<typename... Args>
    void add_cause(file_location first, file_location last, std::format_string<Args...> fmt, Args&&... args)
    {
        _causes.emplace_back(first, last, std::format(std::move(fmt), std::forward<Args>(args)...));
    }

    operator std::unexpected<error_code>() const
    {
        assert(_code.has_value());
        return std::unexpected{_code};
    }

    [[nodiscard]] constexpr file_location first() const noexcept { return _first; }
    [[nodiscard]] constexpr file_location last() const noexcept { return _last; }
    [[nodiscard]] constexpr error_code code() const noexcept { return _code; }
    [[nodiscard]] constexpr std::string_view message() const noexcept { return _message; }
    [[nodiscard]] constexpr auto const& causes() const noexcept { return _causes; }

    void print(std::vector<std::filesystem::path> const& upstream_paths) const;

    [[nodiscard]] std::string to_string(std::vector<std::filesystem::path> const& upstream_paths) const;

private:
    file_location _first = {};
    file_location _last = {};
    error_code _code = {};
    std::string _message = {};
    std::vector<error_cause> _causes = {};
};

}
