
#pragma once

#include "error_codes.hpp"
#include "utility/file_location.hpp"
#include <string>
#include <format>
#include <vector>
#include <cassert>

namespace hl::error {

class cause {
public:

private:
    file_location _first;
    file_location _last;
    std::string _message;
};

class error {
public:

    template<typename... Args>
    void add_cause(file_location first, file_location last, std::format_string<Args...> fmt, Args&&... args)
    {
        _causes.emplace_back(first, last, std::format(std::move(fmt), std::forward<Args>(args)...));
    }

    [[nodiscard]] constexpr file_location first() const noexcept { return _first; }
    [[nodiscard]] constexpr file_location last() const noexcept { return _last; }
    [[nodiscard]] constexpr error_code code() const noexcept { return _code; }
    [[nodiscard]] constexpr std::string_view message() const noexcept { return _message; }
    [[nodiscard]] constexpr auto const& causes() const noexcept { return _causes; }
private:
    file_location _first;
    file_location _last;
    error_code _code = error_code::unknown;
    std::string _message;
    std::vector<cause> _causes;
};

}
