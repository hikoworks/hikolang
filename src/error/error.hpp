#pragma once

#include "error_code.hpp"
#include "utility/file_location.hpp"
#include <string>
#include <format>
#include <vector>
#include <cassert>

namespace hl {

class cause {
public:

private:
    file_location _first;
    file_location _last;
    std::string _message;
};

class error {
public:
    constexpr error() noexcept = default;
    error(error const&) = default;
    error(error&&) = default;
    error& operator=(error const&) = default;
    error& operator=(error&&) = default;

    template<typename... Args>
    error(file_location first, file_location last, error_code code, std::format_string<Args...> fmt, Args&&... args)
        : _first(first), _last(last), _code(code), _message(std::format(std::move(fmt), std::forward<Args>(args)...))
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

private:
    file_location _first = {};
    file_location _last = {};
    error_code _code = {};
    std::string _message = {};
    std::vector<cause> _causes = {};
};

}
