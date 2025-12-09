
#pragma once

#include <string_view>
#include <string>
#include <cstring>
#include <bit>
#include <cstddef>
#include <vector>

namespace hk {

/** Split a string into a vector of strings using a delimiter.
 * 
 * @param str The string to split.
 * @param delimiter The character to use as a delimiter.
 * @return A vector of strings containing the split parts.
 */
[[nodiscard]] std::vector<std::string> split(std::string_view str, char delimiter);

/** Count the length of the prefix of specific characters.
 * 
 * This uses an fast algorithm 
 * 
 * @tparam C The character to look for
 * @param str The string to check the prefix off.
 * @return The number of @a C character at the start of the string.
 */
template<char C>
[[nodiscard]] constexpr size_t prefix_count(std::string_view str)
{
    constexpr auto dots = []{
        auto r = 0uz;
        for (auto i = 0uz; i != sizeof(r); ++i) {
            r <<= 8;
            r |= C;
        }
        return r;
    }();

    auto const fast_size = (str.size() / sizeof(size_t)) * sizeof(size_t);
    auto i = 0uz;
    for (; i != fast_size; i += sizeof(size_t)) {
        size_t value = 0uz;
        std::memcpy(&value, str.data() + i, sizeof(value));
        if (std::endian::native == std::endian::little) {
            value = std::byteswap(value);
        }

        value ^= dots;
        if (value != 0) {
            return i + std::countl_zero(value) / 8;
        }
    }

    for (; i != str.size(); ++i) {
        if (str[i] != C) {
            return i;
        }
    }

    return i;
}

} // namespace hk