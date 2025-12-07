

#include "error_item.hpp"
#include <format>
#include <print>
#include <cassert>

namespace hk {

void error_item::print(line_table const& lines) const
{
    println(stderr, "{}", to_string(lines));
}

[[nodiscard]] static std::string make_text_pointer(size_t left, size_t right)
{
    assert(left <= right);

    auto r = std::string{};
    r.reserve(right + 1);

    auto i = 0;
    for (; i != left; ++i) {
        r += ' ';
    }
    for (; i < right; ++i) {
        r += '~';
    }
    r += '^';
    return r;
}

[[nodiscard]] std::string error_item::to_string(line_table const& lines) const
{
    auto r = std::string{};

    auto const [first_file, first_lineno, first_colno] = lines.get_position(_first);
    auto const [last_file, last_lineno, last_colno] = lines.get_position(_last);

    if (_last == nullptr) {
        r += std::format("{}:{}:{}: {} {}", first_file, first_lineno + 1, first_colno + 1, to_code(code()), code());
    } else {
        r += std::format("{}:{}:{}: {} {}", last_file, last_lineno + 1, last_colno + 1, to_code(code()), code());
    }

    if (auto const& m = message(); not m.empty()) {
        r += std::format(": {}", m);
    }

    if (_last == nullptr) {
        r += std::format("\n{:6} | {}\n", first_lineno + 1, lines.get_line_text(_first));
        r += std::format("       | {}\n", make_text_pointer(first_colno, first_colno));

    } else if (first_file == last_file) {
        r += std::format("\n{:6} | {}\n", last_lineno + 1, lines.get_line_text(_last));
        if (first_lineno == last_lineno) {
            r += std::format("       | {}\n", make_text_pointer(first_colno, last_colno));
        } else {
            r += std::format("       | {}\n", make_text_pointer(0, last_colno));
        }
        
    } else {
        r += std::format("\n{:6} | {}\n", last_lineno + 1, lines.get_line_text(_last));
        r += std::format("       | {}\n", make_text_pointer(0, last_colno));
    }

    return r;
}

} // namespace hk