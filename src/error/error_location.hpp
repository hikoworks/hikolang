
#pragma once

#include "utility/file_location.hpp"
#include "error_list.hpp"

namespace hk {

/** This class holds an error-location in module.
 *
 * If a error occurs outside of the AST tree, this class can be used to report
 * an error inside that module.
 */
class error_location {
public:
    constexpr error_location() noexcept = default;
    constexpr error_location(error_location const&) noexcept = default;
    constexpr error_location(error_location&&) noexcept = default;
    constexpr error_location &operator=(error_location const&) noexcept = default;
    constexpr error_location &operator=(error_location&&) noexcept = default;

    /** Create an error location.
     *
     * @param errors The error list of the module.
     * @param first The location of the first character of the module where an
     *              optional error can appear.
     * @param last One beyond the last character.
     */
    constexpr error_location(error_list &errors, file_location first, file_location last = file_location{})
        : _errors_ptr(&errors), _first(first), _last(last) {}

    /** Add an error to the list. 
     * 
     *  @tparam ErrorCode The error being raised, including the format string.
     *  @param args The arguments to format the error message.
     *  @return A unexpected error containing the error code.
     */
    template<std::derived_from<error_code_and_message_base> Message, typename... Args>
    std::unexpected<error_code> add(Message msg, Args&&... args)
    {
        assert(_errors_ptr != nullptr);

        auto e = error_item{_first, _last, msg.code, Message::fmt, std::forward<Args>(args)...};
        _errors_ptr->push_back(std::move(e));
        return std::unexpected{msg.code};
    }

private:
    error_list *_errors_ptr = nullptr;
    file_location _first = {};
    file_location _last = {};
};

}