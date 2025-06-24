
#pragma once

#include <cstdint>
#include <expected>
#include <cstddef>
#include <cassert>
#include <utility>
#include <array>
#include <bit>
#include <string>

namespace hl {

enum class unicode_error : char32_t {
    /// Invalid UTF-16 surrogate value encoded in UTF-8 stream.
    surrogate = 0xd800,
    /// Found a sole continuation byte.
    continuation_byte = 0xd801,
    /// Found a non-continuation byte in a UTF-8 multi-byte sequence.
    missing_continuation_byte = 0xd802,
    /// UTF-8 multi-byte sequence continues beyond end of buffer.
    buffer_overrun = 0xd803,
    /// Overlong encoding.
    overlong_encoding = 0xd804,
    /// The encoded value was larger than 0x10ffff.
    out_of_range = 0xd805,
    /// Unicode name was not found.
    name_not_found = 0xd806,
};

class utf8_sequence {
public:
    enum class state_type : uint8_t {
        empty = 0x00,
        l1 = 0x01, // 1 byte sequence
        l2 = 0x02, // 2 byte sequence
        l3 = 0x03, // 3 byte sequence
        l4 = 0x04, // 4 byte sequence
        /// Invalid UTF-16 surrogate value encoded in UTF-8 stream.
        surrogate = 0x5,
        /// The encoded value was larger than 0x10ffff.
        out_of_range = 0x6,
    };

    utf8_sequence() noexcept = default;
    utf8_sequence(utf8_sequence const&) noexcept = default;
    utf8_sequence(utf8_sequence&&) noexcept = default;
    utf8_sequence& operator=(utf8_sequence const&) noexcept = default;
    utf8_sequence& operator=(utf8_sequence&&) noexcept = default;

    utf8_sequence(char c0)
    {
        _data[0] = c0;
        _state = state_type::l1;
    }

    utf8_sequence(char c0, char c1)
    {
        _data[0] = c0;
        _data[1] = c1;
        _state = state_type::l2;
    }

    utf8_sequence(char c0, char c1, char c2)
    {
        _data[0] = c0;
        _data[1] = c1;
        _data[2] = c2;
        _state = state_type::l3;
    }

    utf8_sequence(char c0, char c1, char c2, char c3)
    {
        _data[0] = c0;
        _data[1] = c1;
        _data[2] = c2;
        _data[3] = c3;
        _state = state_type::l4;
    }

    utf8_sequence(state_type state) noexcept : _state(state)
    {
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return has_value();
    }

    [[nodiscard]] std::string operator*() const
    {
        return value();
    }

    [[nodiscard]] bool has_value() const noexcept
    {
        return std::to_underlying(_state) <= _data.size();
    }

    [[nodiscard]] std::string value() const
    {
        assert(has_value());
        auto r = std::string{};
        append_to(r);
        return r;
    }

    [[nodiscard]] state_type error() const
    {
        assert(std::to_underlying(_state) > _data.size());
        return _state;
    }

    void append_to(std::string &s) const
    {
        assert(has_value());
        for (uint8_t i = 0; i != std::to_underlying(_state); ++i) {
            s.push_back(_data[i]);
        }
    }
    

private:
    std::array<char, 4> _data = {};
    state_type _state = state_type::empty;
};



enum class utf8_normalization_error {
    /// NFC instance is missing.
    nfc_instance_missing,
    /// Normalization failed.
    normalization_failed,
};

enum class utf8_security_error {
    /// Invalid UTF-8 sequence found.
    invalid_utf8_sequence,
    /// The string is too long to check.
    string_to_long,
    /// Restricted character found.
    restriction_level,
    /// Found invisible character.
    invisible,
    /// Found mixed numbers.
    mixed_numbers,
    /// Found combining character that overlays with a base character.
    hidden_overlay,
    /// Unknown error.
    unknown_error,
};

enum class unicode_name_error {
    could_not_find_name,
};

/** Decode a single Unicode code-point from a UTF-8 stream.
 *
 * @param [in,out] first The iterator to the current position in the UTF-8 stream.
 * @param last The end of the UTF-8 stream.
 * @return The decoded Unicode code-point.
 * @retval 0xd800 Invalid UTF-16 surrogate value encoded in UTF-8 stream.
 * @retval 0xd801 Found a sole continuation byte.
 * @retval 0xd802 Found a non-continuation byte in a UTF-8 multi-byte sequence.
 * @retval 0xd803 UTF-8 multi-byte sequence continues beyond end of buffer.
 * @retval 0xd804 Overlong encoding.
 * @retval 0xd805 The encoded value was larger than 0x10ffff.
 */
[[nodiscard]] char32_t decode_utf8_code_point(char const*& ptr, char const* end);


/** Encode a single Unicode code-point into a UTF-8 sequence.
 * 
 * @param cp The Unicode code-point to encode.
 * @return A UTF-8 sequence representing the code-point.
 * @retval utf8_sequence::state_type::surrogate If the code-point is a surrogate value.
 * @retval utf8_sequence::state_type::out_of_range If the code-point is larger than 0x10ffff.
 */
[[nodiscard]] utf8_sequence encode_utf8_code_point(char32_t cp);

[[nodiscard]] std::string display_utf8_sequence(char const* start, char const* end);

/** Normalize a UTF-8 string to NFC (Normalization Form C).
 * 
 * This function will normalize the string to NFC, which is a canonical composition of characters.
 * 
 * @param s The UTF-8 string to normalize.
 * @return A normalized UTF-8 string.
 * @retval utf8_normalization_error::nfc_instance_missing If the NFC instance is missing.
 * @retval utf8_normalization_error::normalization_failed If the normalization failed.
 */
[[nodiscard]] std::expected<std::string, utf8_normalization_error> normalize_utf8_string(std::string s);

/** Security check a UTF-8 string for invalid characters.
 * 
 * This function will check the string for invalid characters, such as:
 * - Invalid UTF-8 sequences
 * - Restricted characters
 * - Invisible characters
 * - Mixed numbers
 * - Hidden overlays
 * 
 * @param s The UTF-8 string to check.
 * @return An expected value indicating success or an error.
 * @retval utf8_security_error::invalid_utf8_sequence If an invalid UTF-8 sequence was found.
 * @retval utf8_security_error::string_to_long If the string is too long to check.
 * @retval utf8_security_error::restriction_level If a restricted character was found.
 * @retval utf8_security_error::invisible If an invisible character was found.
 * @retval utf8_security_error::mixed_numbers If mixed numbers were found.
 * @retval utf8_security_error::hidden_overlay If a hidden overlay was found.
 * @retval utf8_security_error::unknown_error If an unknown error occurred.
 */
[[nodiscard]] std::expected<void, utf8_security_error> security_check_utf8_string(std::string_view s);

/** Security check a UTF-8 string for invalid characters, splitting an identifier first.
 * 
 * This function first splits the string on underscores and then checks each part for invalid characters.
 * 
 * @see security_check_utf8_string()
 * @param s The UTF-8 string to check.
 * @return An expected value indicating success or an error.
 * @retval utf8_security_error::invalid_utf8_sequence If an invalid UTF-8 sequence was found.
 * @retval utf8_security_error::string_to_long If the string is too long to check.
 * @retval utf8_security_error::restriction_level If a restricted character was found.
 * @retval utf8_security_error::invisible If an invisible character was found.
 * @retval utf8_security_error::mixed_numbers If mixed numbers were found.
 * @retval utf8_security_error::hidden_overlay If a hidden overlay was found.
 * @retval utf8_security_error::unknown_error If an unknown error occurred.
 */
[[nodiscard]] std::expected<void, utf8_security_error> security_check_utf8_string_split_underscore(std::string_view s);

/** Convert a Unicode name to a code-point.
 * 
 * This function will convert a Unicode name to its corresponding code-point.
 * 
 * @param name The Unicode name to convert.
 * @return The corresponding code-point.
 * @retval unicode_name_error::could_not_find_name If the name could not be found.
 */
[[nodiscard]] std::expected<char32_t, unicode_name_error> unicode_name_to_code_point(std::string name);

} // namespace hl
