
#pragma once

#include "error_item.hpp"
#include "hkc_error.hpp"
#include "utility/fixed_string.hpp"
#include <vector>
#include <format>
#include <concepts>
#include <expected>

namespace hk {

class error_list : public std::vector<error_item> {
public:
    using element_type = error_item;

    hkc_error add(line_table const& lines, char const* first, char const* last, hkc_error code, std::string message = std::string{})
    {
        auto const& e = this->emplace_back(code, first, last, std::move(message));
        e.print(lines);
        return code;
    }

    template<typename... Args>
    hkc_error add(line_table const& lines, char const* first, char const* last, hkc_error code, std::format_string<Args...> fmt = {}, Args&&... args)
    {
        return add(lines, first, last, code, std::format(std::move(fmt), std::forward<Args>(args)...));
    }

    template<typename... Args>
    hkc_error add(line_table const& lines, char const* first, hkc_error code, std::format_string<Args...> fmt = {}, Args&&... args)
    {
        return add(lines, first, nullptr, code, std::format(std::move(fmt), std::forward<Args>(args)...));
    }

    template<typename... Args>
    hkc_error add(line_table const& lines, hkc_error code, std::format_string<Args...> fmt = {}, Args&&... args)
    {
        return add(lines, nullptr, nullptr, code, std::format(std::move(fmt), std::forward<Args>(args)...));
    }

private:
};

} // namespace hk
