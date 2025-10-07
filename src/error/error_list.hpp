
#pragma once

#include "error_item.hpp"
#include "error_code.hpp"
#include "tokenizer/file_location.hpp"
#include "utility/fixed_string.hpp"
#include <vector>
#include <format>
#include <concepts>

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
    template<std::derived_from<error_code_and_message_base> Message, typename... Args>
    std::unexpected<error_code> add(file_location first, file_location last, Message msg, Args&&... args)
    {
        auto e = error_item{first, last, msg.code, Message::fmt, std::forward<Args>(args)...};
        this->push_back(std::move(e));
        return std::unexpected{msg.code};
    }

    /** Add an error to the list. 
     * 
     *  @tparam ErrorCode The error being raised, including the format string.
     *  @param args The arguments to format the error message.
     *  @return A unexpected error containing the error code.
     */
    template<std::derived_from<error_code_and_message_base> Message, typename... Args>
    std::unexpected<error_code> add(Message msg, Args&&... args)
    {
        auto e = error_item{msg.code, Message::fmt, std::forward<Args>(args)...};
        this->push_back(std::move(e));
        return std::unexpected{msg.code};
    }

    /** Print error messages.
     * 
     * @param upstream_paths The paths for the error locations.
     */
    void print(std::vector<std::filesystem::path> const& upstream_paths)
    {
        for (auto const& e: *this) {
            e.print(upstream_paths);
        }
    }

    /** Print error messages.
     * 
     * @param upstream_paths The paths for the error locations.
     */
    [[nodiscard]] std::string to_string(std::vector<std::filesystem::path> const& upstream_paths)
    {
        auto s = std::string{};
        for (auto const& e: *this) {
            s += e.to_string(upstream_paths);
            s += '\n';
        }
        return s;
    }

private:
};

}
