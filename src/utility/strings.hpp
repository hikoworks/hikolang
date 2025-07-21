
#pragma once

#include <string_view>
#include <string>
#include <vector>

namespace hk {

/** Split a string into a vector of strings using a delimiter.
 * 
 * @param str The string to split.
 * @param delimiter The character to use as a delimiter.
 * @return A vector of strings containing the split parts.
 */
[[nodiscard]] std::vector<std::string> split(std::string_view str, char delimiter);

} // namespace hk