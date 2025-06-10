
#pragma once

#include <variant>
#include <expected>

/** A tri-state type that can represent a value, an error, or nothing.
 * 
 * @tparam T The type of the value.
 * @tparam E The type of the error.
 */
template<typename T, typename E>
class maybe_expected {
public:
    using value_type = T;
    using error_type = E;
    using unexpected_type = std::unexpected<error_type>;

    constexpr maybe_expected() noexcept = default;
    constexpr maybe_expected(maybe_expected const&) noexcept = default;
    constexpr maybe_expected(maybe_expected&&) noexcept = default;
    constexpr maybe_expected& operator=(maybe_expected const&) noexcept = default;
    constexpr maybe_expected& operator=(maybe_expected&&) noexcept = default;

    constexpr maybe_expected(std::monostate) noexcept : _storage{std::in_place_index_t<0>{}, std::monostate{}} {}
    constexpr maybe_expected(value_type value) noexcept : _storage{std::in_place_index_t<1>{}, std::move(value)} {}
    constexpr maybe_expected(unexpected_type const& error) noexcept : _storage{std::in_place_index_t<2>{}, error.error()} {}
    constexpr maybe_expected(unexpected_type&& error) noexcept : _storage{std::in_place_index_t<2>{}, std::move(error).error()} {}

    constexpr maybe_expected& operator=(std::monostate) noexcept {
        _storage.emplace<0>(std::monostate{});
        return *this;
    }

    constexpr maybe_expected& operator=(value_type value) noexcept {
        _storage.emplace<1>(std::move(value));
        return *this;
    }

    constexpr maybe_expected& operator=(unexpected_type error) noexcept {
        _storage.emplace<2>(std::move(error).error());
        return *this;
    }

    [[nodiscard]] constexpr explicit operator bool() const noexcept {
        return has_value();
    }

    [[nodiscard]] constexpr friend bool operator==(maybe_expected const&, maybe_expected const&) noexcept = default;

    [[nodiscard]] constexpr value_type const& operator*() const& {
        return value();
    }

    [[nodiscard]] constexpr value_type& operator*() & {
        return value();
    }

    [[nodiscard]] constexpr value_type const&& operator*() const&& {
        return value();
    }

    [[nodiscard]] constexpr value_type&& operator*() && {
        return value();
    }

    [[nodiscard]] constexpr value_type const& operator->() const& {
        return value();
    }

    [[nodiscard]] constexpr value_type& operator->() & {
        return value();
    }

    [[nodiscard]] constexpr value_type const&& operator->() const&& {
        return value();
    }

    [[nodiscard]] constexpr value_type&& operator->() && {
        return value();
    }

    [[nodiscard]] constexpr bool is_empty() const noexcept { return _storage.index() == 0; }
    [[nodiscard]] constexpr bool has_value() const noexcept { return _storage.index() == 1; }
    [[nodiscard]] constexpr bool has_error() const noexcept { return _storage.index() == 2; }

    [[nodiscard]] constexpr value_type& value() & {
        assert(has_value());
        return std::get<1>(_storage);
    }

    [[nodiscard]] constexpr value_type const& value() const& {
        assert(has_value());
        return std::get<1>(_storage);
    }

    [[nodiscard]] constexpr value_type&& value() && {
        assert(has_value());
        return std::move(std::get<1>(_storage));
    }

    [[nodiscard]] constexpr value_type const&& value() const&& {
        assert(has_value());
        return std::move(std::get<1>(_storage));
    }

    [[nodiscard]] constexpr error_type const& error() const& {
        assert(has_error());
        return std::get<2>(_storage);
    }

    [[nodiscard]] constexpr error_type& error() & {
        assert(has_error());
        return std::get<2>(_storage);
    }

    [[nodiscard]] constexpr error_type const&& error() const&& {
        assert(has_error());
        return std::move(std::get<2>(_storage));
    }

    [[nodiscard]] constexpr error_type&& error() && {
        assert(has_error());
        return std::move(std::get<2>(_storage));
    }


private:
    using storage_type = std::variant<std::monostate, value_type, error_type>;

    storage_type _storage = storage_type{std::in_place_index_t<0>{}, std::monostate{}};
};
