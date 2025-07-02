#pragma once

#include "error_codes.hpp"
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
    template<typename... Args>
    error(file_location first, file_location last, std::format_string<Args...> fmt, Args&&... args)
        : _first(first), _last(last), _message(std::format(std::move(fmt), std::forward<Args>(args)...))
    {
        _code = parse_code_from_message(_message);
    }

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
    error_code _code = error_code::none;
    std::string _message;
    std::vector<cause> _causes;

    static error_code parse_code_from_message(const std::string& msg) {
        // Assume pattern: [E|W][0-9][0-9][0-9][0-9]: at the start of msg
        if (msg.size() >= 6) {
            char c = msg[0];
            if ((c == 'E' || c == 'W') &&
                std::isdigit(msg[1]) &&
                std::isdigit(msg[2]) &&
                std::isdigit(msg[3]) &&
                std::isdigit(msg[4]) &&
                msg[5] == ':')
            {
                int code_num = (msg[1] - '0') * 1000 +
                               (msg[2] - '0') * 100 +
                               (msg[3] - '0') * 10 +
                               (msg[4] - '0');
                if (c == 'W') {
                    if (code_num >= 0 && code_num <= 9999)
                        return static_cast<error_code>(code_num);
                } else if (c == 'E') {
                    if (code_num >= 0 && code_num <= 9999)
                        return static_cast<error_code>(10000 + code_num);
                }
            }
        }
        throw std::invalid_argument("Could not parse error code from message: " + msg);
    }
};

}
