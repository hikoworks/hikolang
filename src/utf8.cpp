
#include "utf8.hpp"
#include <utility>
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <format>

namespace hl {

[[nodiscard]] char32_t decode_utf8_code_point(char const*& ptr, char const* const end)
{
    // Should only be called if there are code-units to decode.
    assert(ptr != end);

    // Get the first byte.
    auto first_cu = static_cast<uint8_t>(*ptr++);
    if (first_cu < 128) [[likely]] {
        return first_cu;
    }
    first_cu <<= 1;

    if (first_cu < 128) [[unlikely]] {
        return std::to_underlying(decode_utf8_error::continuation_byte);
    }

    int8_t i = -1;
    uint64_t cp = 0;
    #pragma clang loop unroll(disable)
    do {
        first_cu <<= 1;
        ++i;

        if (ptr == end) [[unlikely]] {
            return std::to_underlying(decode_utf8_error::buffer_overrun);
        }

        uint8_t cu = static_cast<uint8_t>(*ptr++) ^ 0x80;
        cp <<= 6;
        cp |= cu;

        if (cu >> 6 != 0) [[unlikely]] {
            return std::to_underlying(decode_utf8_error::missing_continuation_byte);
        }

    } while (first_cu >= 128);

    // Prefix the code-point with the first code-unit.
    // The first code-unit is already stripped from leading bits, but must be shifted back.
    cp |= first_cu << (i * 5 + 4);

    // The loop can be executed maximum 7 times, before first_cu is guaranteed zero.
    //            v
    // 0:       110xxxxx : 2 byte sequence (U+80 - U+7FF)
    // 1:      1110xxxx  : 3 byte sequence (U+0800 - U+FFFF)
    // 2:     11110xxx   : 4 byte sequence (U+010000 - U+10FFFF)
    // 3:    111110xx    : invalid 5 byte sequence
    // 4:   1111110x     : invalid 6 byte sequence
    // 5:  11111110      : invalid 7 byte sequence
    // 6: 11111111       : invalid 8 byte sequence
    constexpr auto minimum_cp_shift = uint64_t{0x00'38'38'38'38'09'04'00};
    auto const minimum_cp = uint64_t{0x80} << static_cast<uint8_t>(minimum_cp_shift >> (i * 8));
    if (cp < minimum_cp) [[unlikely]] {
        return std::to_underlying(decode_utf8_error::overlong_encoding);
    }

    if (cp > 0x10ffff) [[unlikely]] {
        return std::to_underlying(decode_utf8_error::out_of_range);
    }

    if (cp >= 0xd800 and cp <= 0xdfff) [[unlikely]] {
        return std::to_underlying(decode_utf8_error::surrogate);
    }

    return cp;
}

[[nodiscard]] std::expected<std::string, encode_utf8_error> encode_utf8_code_point(char32_t cp)
{
    if (cp < 0x80) [[likely]] {
        return std::string{static_cast<char>(cp)};
    } else if (cp < 0x800) {
        return std::string{
            static_cast<char>(0xc0 | (cp >> 6)),
            static_cast<char>(0x80 | (cp & 0x3f))
        };
    } else if (cp < 0x10000) {
        if (cp >= 0xd800 and cp <= 0xdfff) [[unlikely]] {
            return std::unexpected{encode_utf8_error::surrogate};
        }
        return std::string{
            static_cast<char>(0xe0 | (cp >> 12)),
            static_cast<char>(0x80 | ((cp >> 6) & 0x3f)),
            static_cast<char>(0x80 | (cp & 0x3f))
        };
    } else if (cp <= 0x10ffff) {
        return std::string{
            static_cast<char>(0xf0 | (cp >> 18)),
            static_cast<char>(0x80 | ((cp >> 12) & 0x3f)),
            static_cast<char>(0x80 | ((cp >> 6) & 0x3f)),
            static_cast<char>(0x80 | (cp & 0x3f))
        };
    } else {
        return std::unexpected{encode_utf8_error::out_of_range};
    }
}

[[nodiscard]] std::string display_utf8_sequence(char const* start, char const* end) noexcept
{
    auto r = std::string{};

    for (auto it = start; it != end; ++it) {
        r.append(std::format("{:02x} ", static_cast<unsigned char>(*it)));

        if (static_cast<uint8_t>(*it) < 0x80) {
            break;
        }
    }

    return r;
}

}
