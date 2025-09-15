
#include "base32.hpp"
#include <cassert>
#include <cstdint>

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
    auto r = std::string{};
    r.resize((value.size() * 8 + 4) / 5);

    auto j = 0uz;
    auto i = 0uz;
    while (i != r.size()) {
        auto const byte_nr = j / 8;
        auto const bit_nr = j % 8;
        j += 5;

        auto bits = uint16_t{};
        if (byte_nr + 1 < value.size()) {
            bits |= value[byte_nr + 1];
            bits <<= 8;
        }
        assert(byte_nr < value.size());
        bits |= value[byte_nr];
        bits >>= bit_nr;
        bits &= 0x1f;

        r[i++] = base32_encode_5bit(static_cast<unsigned char>(bits));
    }

    return r;
}


}