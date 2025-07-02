
#pragma once

#include "error/error_code.hpp"
#include "ast/node.hpp"
#include <memory>
#include <variant>
#include <concepts>
#include <cassert>

namespace hl {

template<std::derived_from<ast::node> T>
class parse_result {
public:
    using node_type = T;
    using value_type = std::unique_ptr<node_type>;
    using error_type = error::error_code;

    constexpr parse_result() noexcept = default;
    constexpr parse_result(parse_result const&) noexcept = delete;
    constexpr parse_result(parse_result&&) noexcept = default;
    constexpr parse_result& operator=(parse_result const&) noexcept = delete;
    constexpr parse_result& operator=(parse_result&&) noexcept = default;

    constexpr parse_result(value_type value) noexcept : _result(std::move(value))
    {
        assert(get<value_type>(_result) != nullptr);
    }

    constexpr parse_result(error_type error) noexcept : _result(error) {}

    constexpr parse_result(error::error const& e) noexcept : parse_result(e.code()) {}

    constexpr parse_result(nullptr_t) noexcept : _result(parse_error::none) {}

    [[nodiscard]] constexpr explicit operator bool() const noexcept {
        return has_value();
    }

    [[nodiscard]] constexpr friend bool operator==(parse_result const&, parse_result const&) noexcept = default;

    [[nodiscard]] constexpr value_type const& operator*() const& {
        return value();
    }

    [[nodiscard]] constexpr value_type& operator*() & {
        return value();
    }

    [[nodiscard]] constexpr value_type&& operator*() && {
        return std::move(value());
    }

    [[nodiscard]] constexpr bool has_value() const noexcept {
        return std::holds_alternative<value_type>(_result);
    }

    [[nodiscard]] constexpr bool has_error() const noexcept {
        if (auto error = std::get_if<error_type>(&_result)) {
            return *error != error_code::none;
        }
        return false;
    }

    [[nodiscard]] constexpr value_type const& value() const& {
        assert(has_value());
        return std::get<value_type>(_result);
    }

    [[nodiscard]] constexpr value_type& value() & {
        assert(has_value());
        return std::get<value_type>(_result);
    }

    [[nodiscard]] constexpr value_type&& value() && {
        assert(has_value());
        return std::move(std::get<value_type>(_result));
    }

    [[nodiscard]] constexpr error_type error() const {
        assert(has_error());
        return std::get<error_type>(_result);
    }

private:
    std::variant<value_type, error_type> _result = parse_error::none;
};

}
