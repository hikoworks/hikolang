
#include "build_guard_value.hpp"
#include <format>

namespace hk {


[[nodiscard]] std::string build_guard_value::repr() const
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


} // namespace hk
