

#include "error_item.hpp"
#include <format>
#include <print>
#include <cassert>

namespace hk {

void error_item::print(line_table const& lines) const
{
    println(stderr, "{}", to_string(lines));
}

[[nodiscard]] std::string error_item::to_string(line_table const& lines) const
{
    auto start = lines.get_position(_first);
    auto msg = std::format("{}:{}:{}: {}\n", std::get<0>(start), std::get<1>(start), std::get<2>(start), message());

    return msg;
}

} // namespace hk