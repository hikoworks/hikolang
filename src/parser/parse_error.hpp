
#pragma once

#include <vector>
#include <memory>
#include "ast/node.hpp"
#include "utility/file_location.hpp"

namespace hl {

enum class parse_error {
    /** Not an error, but also means nothing was parsed.
     */
    none = 0,
};

class parse_errors : public std::vector<std::pair<file_location, parse_error>> {
public:
    using element_type = std::pair<file_location, parse_error>;
    using std::vector<element_type>::vector;

    /** Add an error to the list.
     *
     * @param it The iterator where the error occurred.
     * @param message The error message.
     * @return A reference to this parse_errors object.
     */
    template<typename... Args>
    parse_errors& add(file_location first, file_location last, std::format_string<Args...> message, Args&&... args)
    {
        // Here you would typically log the error with the iterator position and message.
        // For simplicity, we just push a dummy error.
        emplace_back(it.first, parse_error::none);
        return *this;
    }
};

}