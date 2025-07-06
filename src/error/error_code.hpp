
#pragma once

#include "utility/fixed_string.hpp"
#include "utility/char_category.hpp"
#include <stdexcept>
#include <cstdint>
#include <string_view>
#include <set>
#include <cassert>
#include <format>

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
            } else if (is_digit(*it)) {
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
};

inline std::set<error_code> all_unique_error_codes = {};

template<fixed_string Fmt>
struct unique_error_code_helper {
    error_code code;

    unique_error_code_helper() {
        code = error_code{Fmt};
        auto const [_, inserted] = all_unique_error_codes.insert(code);

        assert(inserted);
    }
};

template<fixed_string Fmt>
inline unique_error_code_helper<Fmt> unique_error_code = {};

}