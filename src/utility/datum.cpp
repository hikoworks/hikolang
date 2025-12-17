
#include "datum.hpp"
#include <format>
#include <utility>
#include <stdexcept>

namespace hk {

datum::operator bool() const
{
    if (std::holds_alternative<std::monostate>(_value)) {
        return false;
    } else if (auto bp = std::get_if<bool>(&_value)) {
        return *bp;
    } else if (auto ip = std::get_if<long long>(&_value)) {
        return *ip != 0;
    } else if (auto vp = std::get_if<semantic_version>(&_value)) {
        return to_bool(*vp);
    } else if (auto sp = std::get_if<std::string>(&_value)) {
        return not sp->empty();
    } else if (auto vsp = std::get_if<std::vector<std::string>>(&_value)) {
        return not vsp->empty();
    } else {
        std::unreachable();
    }
}


[[nodiscard]] std::string datum::repr() const
{
    using namespace std::literals;

    if (std::holds_alternative<std::monostate>(_value)) {
        return "unknown"s;
    } else if (auto bp = std::get_if<bool>(&_value)) {
        return *bp ? "true"s : "false"s;
    } else if (auto ip = std::get_if<long long>(&_value)) {
        return std::format("{}", *ip);
    } else if (auto vp = std::get_if<semantic_version>(&_value)) {
        return std::format("{}", *vp);
    } else if (auto sp = std::get_if<std::string>(&_value)) {
        return std::format("\"{}\"", *sp);
    } else if (auto vsp = std::get_if<std::vector<std::string>>(&_value)) {
        return std::format("{}", *vsp);
    } else {
        std::unreachable();
    }
}

[[nodiscard]] bool operator==(datum const& lhs, datum const& rhs)
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

    throw std::invalid_argument("==");
}

[[nodiscard]] std::strong_ordering operator<=>(datum const& lhs, datum const& rhs)
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

    throw std::invalid_argument("<=>");
}

[[nodiscard]] bool in(datum const& lhs, datum const& rhs)
{
    if (auto value = std::get_if<std::string>(&lhs._value)) {
        if (auto container = std::get_if<std::vector<std::string>>(&rhs._value)) {
            return std::ranges::contains(*container, *value);
        }
    }

    throw std::invalid_argument("in");
}

} // namespace hk
