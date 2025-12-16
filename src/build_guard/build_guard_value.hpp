
#pragma once

#include "utility/semantic_version.hpp"
#include <string>
#include <variant>
#include <vector>
#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <optional>

namespace hk {

class build_guard_value {
public:
    enum class bin_op { _and, _or, in, not_in, eq, ne, lt, gt, le, ge };
    enum class un_op { _not };

    constexpr build_guard_value(build_guard_value const&) = default;
    constexpr build_guard_value(build_guard_value&&) = default;
    constexpr build_guard_value& operator=(build_guard_value const&) = default;
    constexpr build_guard_value& operator=(build_guard_value&&) = default;
    constexpr build_guard_value() = default;
    constexpr build_guard_value(bool value) : _value(value) {}
    constexpr build_guard_value(long long value) : _value(value) {}
    constexpr build_guard_value(semantic_version value) : _value(value) {}
    constexpr build_guard_value(std::string value) : _value(std::move(value)) {}

    [[nodiscard]] std::string repr() const;

    [[nodiscard]] constexpr friend bool to_bool(build_guard_value const& rhs)
    {
        if (std::holds_alternative<std::monostate>(rhs._value)) {
            return false;
        } else if (auto bp = std::get_if<bool>(&rhs._value)) {
            return *bp;
        } else if (auto ip = std::get_if<long long>(&rhs._value)) {
            return *ip != 0;
        } else if (auto vp = std::get_if<semantic_version>(&rhs._value)) {
            return to_bool(*vp);
        } else if (auto sp = std::get_if<std::string>(&rhs._value)) {
            return not sp->empty();
        } else if (auto vsp = std::get_if<std::vector<std::string>>(&rhs._value)) {
            return not vsp->empty();
        } else {
            std::unreachable();
        }
    }

    [[nodiscard]] constexpr friend bool binary_and(build_guard_value const& lhs, build_guard_value const& rhs)
    {
        return to_bool(lhs) and to_bool(rhs);
    }

    [[nodiscard]] constexpr friend bool binary_or(build_guard_value const& lhs, build_guard_value const& rhs)
    {
        return to_bool(lhs) or to_bool(rhs);
    }

    [[nodiscard]] constexpr friend std::optional<bool> binary_in(build_guard_value const& lhs, build_guard_value const& rhs)
    {
        if (auto value = std::get_if<std::string>(&lhs._value)) {
            if (auto container = std::get_if<std::vector<std::string>>(&rhs._value)) {
                return std::ranges::contains(*container, *value);
            }
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr friend std::optional<bool> binary_eq(build_guard_value const& lhs, build_guard_value const& rhs)
    {
        if (std::holds_alternative<std::monostate>(lhs._value) and std::holds_alternative<std::monostate>(rhs._value)) {
            return true;
        }

        if (auto lhs_ = std::get_if<bool>(&lhs._value)) {
            if (auto rhs_ = std::get_if<bool>(&rhs._value)) {
                return *lhs_ == *rhs_;
            }
        }

        if (auto lhs_ = std::get_if<long long>(&lhs._value)) {
            if (auto rhs_ = std::get_if<long long>(&rhs._value)) {
                return *lhs_ == *rhs_;
            }
        }

        if (auto lhs_ = std::get_if<semantic_version>(&lhs._value)) {
            if (auto rhs_ = std::get_if<semantic_version>(&rhs._value)) {
                return *lhs_ == *rhs_;
            }
        }

        if (auto lhs_ = std::get_if<std::string>(&lhs._value)) {
            if (auto rhs_ = std::get_if<std::string>(&rhs._value)) {
                return *lhs_ == *rhs_;
            }
        }

        if (auto lhs_ = std::get_if<std::vector<std::string>>(&lhs._value)) {
            if (auto rhs_ = std::get_if<std::vector<std::string>>(&rhs._value)) {
                return *lhs_ == *rhs_;
            }
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr friend std::optional<std::strong_ordering>
    binary_cmp(build_guard_value const& lhs, build_guard_value const& rhs)
    {
        if (auto lhs_ = std::get_if<bool>(&lhs._value)) {
            if (auto rhs_ = std::get_if<bool>(&rhs._value)) {
                return *lhs_ <=> *rhs_;
            }
        }

        if (auto lhs_ = std::get_if<long long>(&lhs._value)) {
            if (auto rhs_ = std::get_if<long long>(&rhs._value)) {
                return *lhs_ <=> *rhs_;
            }
        }

        if (auto lhs_ = std::get_if<semantic_version>(&lhs._value)) {
            if (auto rhs_ = std::get_if<semantic_version>(&rhs._value)) {
                return *lhs_ <=> *rhs_;
            }
        }

        if (auto lhs_ = std::get_if<std::string>(&lhs._value)) {
            if (auto rhs_ = std::get_if<std::string>(&rhs._value)) {
                return *lhs_ <=> *rhs_;
            }
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr friend std::optional<build_guard_value>
    binary_op(bin_op op, build_guard_value const& lhs, build_guard_value const& rhs)
    {
        switch (op) {
        case bin_op::_and:
            return binary_and(lhs, rhs);
        case bin_op::_or:
            return binary_or(lhs, rhs);
        case bin_op::in:
            return binary_in(lhs, rhs);
        case bin_op::not_in:
            if (auto const r = binary_in(lhs, rhs)) {
                return not *r;
            } else {
                return std::nullopt;
            }
        case bin_op::eq:
            return binary_eq(lhs, rhs);
        case bin_op::ne:
            if (auto const r = binary_eq(lhs, rhs)) {
                return not *r;
            } else {
                return std::nullopt;
            }
        case bin_op::lt:
            if (auto const r = binary_cmp(lhs, rhs)) {
                return *r == std::strong_ordering::less;
            } else {
                return std::nullopt;
            }
        case bin_op::gt:
            if (auto const r = binary_cmp(lhs, rhs)) {
                return *r == std::strong_ordering::greater;
            } else {
                return std::nullopt;
            }
        case bin_op::le:
            if (auto const r = binary_cmp(lhs, rhs)) {
                return *r != std::strong_ordering::greater;
            } else {
                return std::nullopt;
            }
        case bin_op::ge:
            if (auto const r = binary_cmp(lhs, rhs)) {
                return *r != std::strong_ordering::less;
            } else {
                return std::nullopt;
            }
        }
        std::unreachable();
    }

    [[nodiscard]] constexpr friend std::optional<build_guard_value> unary_op(un_op op, build_guard_value const& rhs)
    {
        switch (op) {
        case un_op::_not:
            return not to_bool(rhs);
        }
        std::unreachable();
    }

private:
    std::variant<std::monostate, bool, long long, std::string, semantic_version, std::vector<std::string>> _value =
        std::monostate{};
};

} // namespace hk
