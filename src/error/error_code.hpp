
#pragma once

#include "utility/fixed_string.hpp"
#include "utility/char_category.hpp"

namespace hl {

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

    constexpr error_code(std::string_view str) noexcept
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
};

inline std::set<error_code> all_error_codes = {};

template<fixed_string Fmt>
struct error_message_helper {
    error_message_helper() {
        auto const code = error_code{Fmt};
        auto const [_, inserted] = error_messages.insert(code);

        assert(inserted);
    }
};

template<fixed_string Fmt>
inline error_message_helper<Fmt> error_message = {};

}