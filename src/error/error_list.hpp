
#pragma once

#include "error.hpp"
#include "error_codes.hpp"
#include "utility/file_location.hpp"
#include "utility/fixed_string.hpp"
#include <vector>
#include <format>

namespace hl {

class error_list : public std::vector<error> {
public:
    using element_type = error;

    /** Add an error to the list. 
     * 
     *  @param first The first file location where the error occurred.
     *  @param last The last file location where the error occurred.
     *  @param code The error code.
     *  @param fmt The format string for the error message.
     *  @param args The arguments to format the error message.
     *  @return A reference to the newly added error.
     */
    template<fixed_string Fmt, typename... Args>
    error &add(file_location first, file_location last, Args&&... args)
    {
        auto message = std::format(std::move(fmt), std::forward<Args>(Fmt)...);
        return emplace_back(first, last, code, std::move(message));
    }

private:
};

}
