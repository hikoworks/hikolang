
#include "unicode.hpp"
#include <utility>
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <format>
#include <unicode/utypes.h>
#include <unicode/uchar.h>

#include <unicode/normalizer2.h>
#include <unicode/stringpiece.h>
#include <unicode/uspoof.h>
#include <unicode/ubidi.h>
#include <unicode/uset.h>
#include <unicode/parseerr.h>
#include <unicode/localpointer.h>
#include <unicode/unistr.h>
#include <unicode/uniset.h>

namespace hk {

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
        return std::to_underlying(unicode_error::continuation_byte);
    }

    int8_t i = -1;
    uint64_t cp = 0;
    #pragma clang loop unroll(disable)
    do {
        first_cu <<= 1;
        ++i;

        if (ptr == end) [[unlikely]] {
            return std::to_underlying(unicode_error::buffer_overrun);
        }

        uint8_t cu = static_cast<uint8_t>(*ptr++) ^ 0x80;
        cp <<= 6;
        cp |= cu;

        if (cu >> 6 != 0) [[unlikely]] {
            return std::to_underlying(unicode_error::missing_continuation_byte);
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
        return std::to_underlying(unicode_error::overlong_encoding);
    }

    if (cp > 0x10ffff) [[unlikely]] {
        return std::to_underlying(unicode_error::out_of_range);
    }

    if (cp >= 0xd800 and cp <= 0xdfff) [[unlikely]] {
        return std::to_underlying(unicode_error::surrogate);
    }

    return cp;
}

[[nodiscard]] utf8_sequence encode_utf8_code_point(char32_t cp)
{
    if (cp < 0x80) [[likely]] {
        return utf8_sequence{static_cast<char>(cp)};

    } else if (cp < 0x800) {
        return utf8_sequence{
            static_cast<char>(0xc0 | (cp >> 6)),
            static_cast<char>(0x80 | (cp & 0x3f))
        };
    } else if (cp < 0x10000) {
        if (cp >= 0xd800 and cp <= 0xdfff) [[unlikely]] {
            return utf8_sequence{utf8_sequence::state_type::surrogate};
        }
        return utf8_sequence{
            static_cast<char>(0xe0 | (cp >> 12)),
            static_cast<char>(0x80 | ((cp >> 6) & 0x3f)),
            static_cast<char>(0x80 | (cp & 0x3f))
        };
    } else if (cp <= 0x10ffff) {
        return utf8_sequence{
            static_cast<char>(0xf0 | (cp >> 18)),
            static_cast<char>(0x80 | ((cp >> 12) & 0x3f)),
            static_cast<char>(0x80 | ((cp >> 6) & 0x3f)),
            static_cast<char>(0x80 | (cp & 0x3f))
        };
    } else {
        return utf8_sequence{utf8_sequence::state_type::out_of_range};
    }
}

[[nodiscard]] std::string display_utf8_sequence(char const* start, char const* end)
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

[[nodiscard]] std::expected<std::string, utf8_normalization_error> normalize_utf8_string(std::string s)
{
    auto r = std::move(s);
    auto r_source = icu::StringPiece{r};

    UErrorCode error_code = U_ZERO_ERROR;
    auto* const nfc = icu::Normalizer2::getNFCInstance(error_code);
    if (U_FAILURE(error_code)) {
        return std::unexpected{utf8_normalization_error::nfc_instance_missing};
    }
    assert(nfc != nullptr);


    error_code = U_ZERO_ERROR;
    auto const check_value = nfc->isNormalizedUTF8(r_source, error_code);
    if (U_FAILURE(error_code)) {
        return std::unexpected{utf8_normalization_error::normalization_failed};
    }
    if (check_value) {
        // This is single move.
        return r;
    }

    auto normalized_string = std::string{};
    normalized_string.reserve(r.size());
    auto  normalized_string_sink = icu::StringByteSink<std::string>{&normalized_string};

    error_code = U_ZERO_ERROR;
    nfc->normalizeUTF8(0, r_source, normalized_string_sink, nullptr, error_code);
    if (U_FAILURE(error_code)) {
        return std::unexpected{utf8_normalization_error::normalization_failed};
    }

    r = std::move(normalized_string);
    return r;
}

struct spoof_checker_type {
    USpoofChecker* checker = nullptr;

    spoof_checker_type()
    {
        UErrorCode error = U_ZERO_ERROR;

        checker = uspoof_open(&error);
        if (U_FAILURE(error)) {
            throw std::runtime_error{"Failed to create USpoofChecker instance."};
        }
        assert(checker != nullptr);

        auto const checks = USPOOF_ALL_CHECKS & ~(USPOOF_CHAR_LIMIT | USPOOF_SINGLE_SCRIPT | USPOOF_ANY_CASE);
        uspoof_setChecks(checker, checks, &error);
        if (U_FAILURE(error)) {
            throw std::runtime_error{"Failed to set checks on the USpoofChecker instance."};
        }
    }

    ~spoof_checker_type()
    {
        if (checker != nullptr) {
            uspoof_close(checker);
            checker = nullptr;
        }
    }
};

static auto spoof_checker = spoof_checker_type{};

[[nodiscard]] std::expected<void, utf8_security_error> security_check_utf8_string(std::string_view s)
{
    assert(spoof_checker.checker != nullptr);

    if (s.empty()) {
        // Empty string is always valid.
        return {};
    }

    if (s.size() > 0x7fff'ffff) {
        // The maximum length of a string is 0x7fffffff, so this is an error.
        return std::unexpected{utf8_security_error::string_to_long};
    }

    UErrorCode error_code = U_ZERO_ERROR;
    auto const check_value = uspoof_checkUTF8(spoof_checker.checker, s.data(), static_cast<int32_t>(s.size()), NULL, &error_code);
    if (U_FAILURE(error_code)) {
        return std::unexpected{utf8_security_error::invalid_utf8_sequence};
    }

    if ((check_value & USPOOF_ALL_CHECKS) != 0) {
        // If any of the checks failed, we return an error.
        if (check_value & USPOOF_RESTRICTION_LEVEL) {
            return std::unexpected{utf8_security_error::restriction_level};
        }
        if (check_value & USPOOF_INVISIBLE) {
            return std::unexpected{utf8_security_error::invisible};
        }
        if (check_value & USPOOF_MIXED_NUMBERS) {
            return std::unexpected{utf8_security_error::mixed_numbers};
        }
        if (check_value & USPOOF_HIDDEN_OVERLAY) {
            return std::unexpected{utf8_security_error::hidden_overlay};
        }

        return std::unexpected{utf8_security_error::unknown_error};
    }

    return {};
}

[[nodiscard]] std::expected<void, utf8_security_error> security_check_utf8_string_split_underscore(std::string_view s)
{
    auto i = 0uz;
    auto j = s.find('_', i);
    while (j != std::string_view::npos) {
        auto sub = s.substr(i, j - i);

        auto result = security_check_utf8_string(sub);
        if (not result) {
            return std::unexpected{result.error()};
        }

        i = j + 1;
        j = s.find('_', i);
    }

    auto sub = s.substr(i);
    auto result = security_check_utf8_string(sub);
    if (not result) {
        return std::unexpected{result.error()};
    }

    return {};
}

[[nodiscard]] std::expected<char32_t, unicode_name_error> unicode_name_to_code_point(std::string name)
{
    UErrorCode error_code = U_ZERO_ERROR;
    auto cp = u_charFromName(U_UNICODE_CHAR_NAME, name.c_str(), &error_code);
    if (U_SUCCESS(error_code)) {
        // If the code-point is valid, return it.
        return cp;
    }

    error_code = U_ZERO_ERROR;
    cp = u_charFromName(U_CHAR_NAME_ALIAS, name.c_str(), &error_code);
    if (U_SUCCESS(error_code)) {
        // If the code-point is valid, return it.
        return cp;
    }
    
    return std::unexpected{unicode_name_error::could_not_find_name};
}

}
