
#pragma once

#include <string>
#include <string_view>
#include <array>

namespace hk {

/** Encode 
 * 
 */
[[nodiscard]] std::array<char, 32> sha256(std::string_view str);

}
