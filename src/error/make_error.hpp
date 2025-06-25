
#pragma once

#include "error.hpp"

namespace hl {

inline std::vector<error> all_errors = {};

template<typename... Args>
error_code
make_error(file_location first, file_location last, error_code code, std::format_string<std::string> fmt, Args&&... args)
{
    all_errors.emplace_back(first, last, code, std::format(std::move(fmt), std::forward<Args>(args)...));
    return code;
}

} // namespace hl
