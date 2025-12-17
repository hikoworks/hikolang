
#pragma once

#include "semantic_version.hpp"
#include <string>
#include <variant>
#include <vector>
#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <optional>

namespace hk {

class datum {
public:
    constexpr datum(datum const&) = default;
    constexpr datum(datum&&) = default;
    constexpr datum& operator=(datum const&) = default;
    constexpr datum& operator=(datum&&) = default;
    constexpr datum() = default;
    constexpr datum(bool value) : _value(value) {}
    constexpr datum(long long value) : _value(value) {}
    constexpr datum(semantic_version value) : _value(value) {}
    constexpr datum(std::string value) : _value(std::move(value)) {}

    explicit operator bool() const;

    [[nodiscard]] std::string repr() const;

    friend bool in(datum const& lhs, datum const& rhs);
    friend bool operator==(datum const& lhs, datum const& rhs);
    friend std::strong_ordering operator<=>(datum const& lhs, datum const& rhs);

private:
    std::variant<std::monostate, bool, long long, std::string, semantic_version, std::vector<std::string>> _value =
        std::monostate{};
};

[[nodiscard]] bool operator==(datum const& lhs, datum const& rhs);
[[nodiscard]] std::strong_ordering operator<=>(datum const& lhs, datum const& rhs);
[[nodiscard]] bool in(datum const& lhs, datum const& rhs);

} // namespace hk
