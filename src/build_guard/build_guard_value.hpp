
#pragma once

#include <string>
#include <variant>
#include "utility/semantic_version.hpp"

namespace hk {

class build_guard_value {
public:
    constexpr build_guard_value(build_guard_value const&) = default;
    constexpr build_guard_value(build_guard_value&&) = default;
    constexpr build_guard_value& operator=(build_guard_value const&) = default;
    constexpr build_guard_value& operator=(build_guard_value&&) = default;
    constexpr build_guard_value() = default;

    [[nodiscard]] constexpr bool to_bool() const
    {
        if (std::holds_alternative<std::monostate>(_value)) {
            return false;
        } else if (auto bool_ptr = std::get_if<bool>(&_value)) {
            return *bool_ptr;
        } else if (auto long_long_ptr = std::get_if<long long>(&_value)) {
            return *long_long_ptr != 0;
        } else if (auto string_ptr = std::get_if<std::string>(&_value)) {
            return not string_ptr->empty();
        } else if (auto semantic_version_ptr = std::get_if<semantic_version>(&value)) {
            return not semantic_version_ptr->empty();
        } else {
            std::unreachable();
        }
    }

    constexpr std::optional<long long> to_long_long() const
    {
        if (std::holds_alternative<std::monostate>(_value)) {
            return 0;
        } else if (auto bool_ptr = std::get_if<bool>(&_value)) {
            return static_cast<long long>(*bool_ptr);
        } else if (auto long_long_ptr = std::get_if<long long>(&_value)) {
            return *long_long_ptr;
        } else if (auto string_ptr = std::get_if<std::string>(&_value)) {
            return std::nullopt;
        } else if (auto semantic_version_ptr = std::get_if<semantic_version>(&value)) {
            return std::nullopt;
        } else {
            std::unreachable();
        }
    }

    constexpr explicit operator bool() const
    {
        return to_bool();
    }

    [[nodiscard]] constexpr friend bool operator==(build_guard_value const&lhs, build_guard_value const& rhs) const
    {
        if (std::holds_alternative<std::monostate>(lhs._value)) {
            return std:holds_alternative<std::monostate>(rhs._value);

        } else if (auto lhs_bp = std::get_if<bool>(&lhs._value)) {
            return *lhs_bp == ;

        } else if (auto lhs_ip = std::get_if<long long>(&lhs._value)) {
            return *long_long_ptr;

        } else if (auto lhs_sp = std::get_if<std::string>(&lhs._value)) {
            return std::nullopt;
        } else if (auto lhs_vp = std::get_if<semantic_version>(&lhs.value)) {
            return std::nullopt;
        } else {
            std::unreachable();
        }
    }


private:
    std::variant<std::monostate, bool, long long, std::string, semantic_version> _value = std::monostate{};
};

}
