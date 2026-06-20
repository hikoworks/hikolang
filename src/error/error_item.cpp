

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
    auto const [first_file, first_lineno, first_colno, location_string] = lines.get_error_location(_first, _last);

    auto r = std::format("{}:{}:{}: {} {}", first_file, first_lineno + 1, first_colno + 1, to_code(code()), code());

    if (auto const& m = message(); not m.empty()) {
        r += std::format(": {}", m);
    }

    r += location_string;
    return r;
}

} // namespace hk