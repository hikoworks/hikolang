
#pragma once

#include "parse_error.hpp"
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
    using error_type = parse_error;

    constexpr parse_result() noexcept = default;
    constexpr parse_result(parse_result const&) noexcept = delete;
    constexpr parse_result(parse_result&&) noexcept = default;
    constexpr parse_result& operator=(parse_result const&) noexcept = delete;
    constexpr parse_result& operator=(parse_result&&) noexcept = default;

    constexpr parse_result(value_type value) noexcept : _result(std::move(value)) {}
    constexpr parse_result(erandor_type error) noexcept : _result(error) {}

    [[nodiscard]] constexpr bool has_value() const noexcept {
        return std::holds_alternative<value_type>(_result);
    }

    [[nodiscard]] constexpr bool has_error() const noexcept {
        if (auto error = std::get_if<error_type>(&_result)) {
            return *error != parse_error::none;
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
