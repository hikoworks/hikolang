
#pragma once

#include <string>
#include <string_view>
#include <span>

namespace hk {

/** Encode 5 bits.
 * 
 * @note It is UNDEFINED BEHAVIOR when other bits than the bottom 5 bits.
 * @param value Lower 5 bits to encode.
 * @return 5 bit encoded as a RFC 4648 character.
 */
[[nodiscard]] char base32_encode_5bit(unsigned char value);

/** Encode a char span to base32.
 * 
 */
[[nodiscard]] std::string base32_encode(std::span<char const> value);

[[nodiscard]] inline std::string base32_encode(std::string_view value)
{
    return base32_encode(std::span{value.data(), value.size()});
}


}