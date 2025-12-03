
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

    template<typename... Args>
    hkc_error add(char const* first, char const* last, hkc_error code, std::format_string<Args...> fmt, Args&&... args)
    {
        this->emplace_back(first, last, code, std::move(fmt), std::forward<Args>(args)...);
        return code;
    }

    template<typename... Args>
    hkc_error add(char const* first, hkc_error code, std::format_string<Args...> fmt, Args&&... args)
    {
        this->emplace_back(first, code, std::move(fmt), std::forward<Args>(args)...);
        return code;
    }

    template<typename... Args>
    hkc_error add(hkc_error code, std::format_string<Args...> fmt, Args&&... args)
    {
        this->emplace_back(code, std::move(fmt), std::forward<Args>(args)...);
        return code;
    }

    hkc_error add(char const* first, char const* last, hkc_error code)
    {
        this->emplace_back(first, last, code);
        return code;
    }

    hkc_error add(char const* first, hkc_error code)
    {
        this->emplace_back(first, code);
        return code;
    }

    hkc_error add(hkc_error code)
    {
        this->emplace_back(code);
        return code;
    }

    /** Print error messages.
     *
     * @param upstream_paths The paths for the error locations.
     */
    void print(line_table const& lines)
    {
        for (auto const& e : *this) {
            e.print(lines);
        }
    }

    /** Print error messages.
     *
     * @param upstream_paths The paths for the error locations.
     */
    [[nodiscard]] std::string to_string(line_table const& lines)
    {
        auto s = std::string{};
        for (auto const& e : *this) {
            s += e.to_string(lines);
            s += '\n';
        }
        return s;
    }

private:
};

} // namespace hk
