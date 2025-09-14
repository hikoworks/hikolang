
#pragma once

#include "error_item.hpp"
#include "error_code.hpp"
#include "utility/file_location.hpp"
#include "utility/fixed_string.hpp"
#include <vector>
#include <format>

namespace hk {

class error_list : public std::vector<error_item> {
public:
    using element_type = error_item;

    /** Add an error to the list. 
     * 
     *  @tparam ErrorCode The error being raised, including the format string.
     *  @param first The first file location where the error occurred.
     *  @param last The last file location where the error occurred.
     *  @param args The arguments to format the error message.
     *  @return A unexpected error containing the error code.
     */
    template<typename ErrorType, typename... Args>
    std::unexpected<error_code> add(file_location first, file_location last, Args&&... args)
    {
        auto e = error_item{first, last, ErrorType::code, ErrorType::fmt, std::forward<Args>(args)...};
        this->push_back(std::move(e));
        return std::unexpected{ErrorType::code};
    }

    /** Add an error to the list. 
     * 
     *  @tparam ErrorCode The error being raised, including the format string.
     *  @param args The arguments to format the error message.
     *  @return A unexpected error containing the error code.
     */
    template<typename ErrorType, typename... Args>
    std::unexpected<error_code> add(Args&&... args)
    {
        auto e = error_item{ErrorType::code, ErrorType::fmt, std::forward<Args>(args)...};
        this->push_back(std::move(e));
        return std::unexpected{ErrorType::code};
    }

private:
};

}
