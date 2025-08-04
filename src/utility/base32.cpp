
#include "base32.hpp"
#include <cassert>

namespace hk {

[[nodiscard]] char base32_encode_5bit(unsigned char value)
{
    assert(value < 0x20);
    if (value < 26) {
        value += 'a';
    } else {
        value -= 26;
        value += '2';
    }
    return static_cast<char>(value);
}

[[nodiscard]] std::string base32_encode(std::span<char const> value)
{
    auto const r_size = (value.size() * 8 + 4) / 5;
    auto r = std::string{};
    r.resize(r_size);

    auto j = 0uz;
    for (auto i = 0uz; i != r_size; ++i, j += 5) {
        auto const byte_nr = j / 8;
        auto const bit_nr = j % 8;

        auto bits = uint16_t{0};
        bits |= value[byte_nr + 1];
        bits <<= 8;
        bits |= value[byte_nr];
        bits >>= bit_nr;
        bits &= 0x1f;

        r[i] = base32_encode_5bit(static_cast<unsigned char>(bits));
    }

    return r;
}


}