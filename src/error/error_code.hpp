
#pragma once

#include "utility/fixed_string.hpp"
#include <stdexcept>
#include <cstdint>
#include <string_view>
#include <set>
#include <cassert>
#include <format>
#include <print>

namespace hk {

struct error_code {
    char kind = '\0';
    uint32_t code = 0;

    constexpr error_code() noexcept = default;
    constexpr error_code(error_code const&) noexcept = default;
    constexpr error_code(error_code&&) noexcept = default;
    constexpr error_code& operator=(error_code const&) noexcept = default;
    constexpr error_code& operator=(error_code&&) noexcept = default;
    [[nodiscard]] constexpr friend bool operator==(error_code const&, error_code const&) noexcept = default;
    [[nodiscard]] constexpr friend auto operator<=>(error_code const&, error_code const&) noexcept = default;

    constexpr error_code(uint16_t code, char kind) noexcept : code(code), kind(kind) {}

    constexpr error_code(std::string_view str)
    {
        auto it = str.begin();
        if (it == str.end()) {
            throw std::invalid_argument("Error code string cannot be empty");
        }
        kind = *it++;
        if (kind != 'E' && kind != 'W' && kind != 'I') {
            throw std::invalid_argument("Error code must start with 'E', 'W', or 'I'");
        }

        while (it != str.end()) {
            if (*it == ':') {
                return;
            } else if (*it >= '0' and *it <= '9') {
                code *= 10;
                code += *it - '0';
                ++it;
            }
        }

        throw std::invalid_argument("Error code string must contain a colon ':' after the numeric part");
    }

    [[nodiscard]] constexpr bool has_value() const noexcept
    {
        return code != 0 or kind != '\0';
    }

    constexpr explicit operator bool() const noexcept
    {
        return has_value();
    }

    [[nodiscard]] friend std::string to_string(error_code rhs)
    {
        return std::format("{}{:04}", rhs.kind, rhs.code);
    }
};


struct error_code_and_message_base {
    error_code code = {};

    error_code_and_message_base(error_code code) : code(code)
    {
        auto [it, inserted] = all_error_codes.insert(code);
        if (not inserted) {
            std::println(stderr, "Error: found collision in error-list: {}", to_string(code));
            std::terminate();
        }
    }

    inline static std::set<error_code> all_error_codes = {};
};

template<fixed_string Fmt>
struct error_code_and_message : public error_code_and_message_base {
    constexpr static decltype(Fmt) fmt = Fmt;

    error_code_and_message() : error_code_and_message_base(error_code{fmt})
    {
    }
};


}