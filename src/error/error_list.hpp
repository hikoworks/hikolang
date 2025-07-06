
#pragma once

#include "error.hpp"
#include "error_code.hpp"
#include "utility/file_location.hpp"
#include "utility/fixed_string.hpp"
#include <vector>
#include <format>

namespace hk {

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
     *  @return A unexpected error containing the error code.
     */
    template<fixed_string Fmt, typename... Args>
    std::unexpected<error_code> add(file_location first, file_location last, Args&&... args)
    {
        auto const code = unique_error_code<Fmt>.code;
        assert(code.has_value());

        auto e = error{first, last, code, Fmt, std::forward<Args>(args)...};
        this->push_back(std::move(e));
        return std::unexpected{code};
    }

private:
};

}
