
#pragma once

#include "utility/unicode.hpp"
#include "utility/file_location.hpp"
#include "utility/semantic_version.hpp"
#include <string>
#include <string_view>
#include <cstddef>
#include <cassert>
#include <variant>
#include <format>

namespace hl {

class token {
public:
    enum class kind_type {
        empty,
        error,
        simple,
        identifier,
        _operator,
        comment,
        documentation,
        back_documentation,
        string_literal,
        character_literal,
        quote_literal,
        integer_literal,
        float_literal,
        version_literal,
        positional_argument,
        line_directive,
        scram_directive,
        bracketed_string_literal,
    };

    static constexpr kind_type empty = kind_type::empty;
    static constexpr kind_type error = kind_type::error;
    static constexpr kind_type simple = kind_type::simple;
    static constexpr kind_type identifier = kind_type::identifier;
    static constexpr kind_type _operator = kind_type::_operator;
    static constexpr kind_type comment = kind_type::comment;
    static constexpr kind_type documentation = kind_type::documentation;
    static constexpr kind_type back_documentation = kind_type::back_documentation;
    static constexpr kind_type string_literal = kind_type::string_literal;
    static constexpr kind_type character_literal = kind_type::character_literal;
    static constexpr kind_type quote_literal = kind_type::quote_literal;
    static constexpr kind_type integer_literal = kind_type::integer_literal;
    static constexpr kind_type float_literal = kind_type::float_literal;
    static constexpr kind_type version_literal = kind_type::version_literal;
    static constexpr kind_type positional_argument = kind_type::positional_argument;
    static constexpr kind_type line_directive = kind_type::line_directive;
    static constexpr kind_type scram_directive = kind_type::scram_directive;
    static constexpr kind_type bracketed_string_literal = kind_type::bracketed_string_literal;

    /** The location in the file where the first character of a token is located.
     */
    file_location first = {};

    /** One position after the location of the last character of the token.
     */
    file_location last = {};

    kind_type kind = kind_type::empty;

    /** The significant part of the token.
     */
    std::string text = {};

    /** Documentation for the token.
     */
    std::string doc_text = {};

    /** Default constructor.
     * 
     * This constructor initializes the token to an empty state.
     */
    constexpr token() noexcept = default;

    /** Create a token with the first character's location.
     * 
     * This is used for tokens when the kind is not known yet.
     *
     * @param first The location of the first character of the token.
     */
    constexpr token(file_location const &first) noexcept :
        first(first), last(first), kind(kind_type::empty)
    {
    }

    /** Create a token with the first character's location and kind.
     * 
     * @param first The location of the first character of the token.
     * @param kind The kind of the token.
     */
    constexpr token(file_location const &first, kind_type kind) noexcept :
        first(first), last(first), kind(kind)
    {
    }

    /** Create a simple, single character, token.
     * 
     * This is used for single character tokens like brackets, separators, end-of-file, etc.
     * 
     * The following characters are considered simple tokens:
     *  - `;` (Semicolon)
     *  - `,` (Comma)
     *  - `{` (Left Brace)
     *  - `}` (Right Brace)
     *  - `[` (Left Square Bracket)
     *  - `]` (Right Square Bracket)
     *  - `(` (Left Parenthesis)
     *  - `)` (Right Parenthesis)
     *  - `\n` (New Line, This represents all vertical space characters)
     *  - `\0` (End of File)
     * 
     * @param first The location of the first character of the token.
     * @param c The character of the token.
     * @return A token representing the single character.
     */
    constexpr token(file_location const &first, char c) noexcept :
        first(first), last(first), kind(kind_type::simple), text(1, c)
    {
        assert(static_cast<uint8_t>(c) <= 0x7f);
        assert(c != '\v');
        assert(c != '\f');
        assert(c != '\r');
        last.advance(c, '\0');
    }


    constexpr token(token&&) noexcept = default;
    constexpr token(const token&) noexcept = default;
    constexpr token& operator=(token&&) noexcept = default;
    constexpr token& operator=(const token&) noexcept = default;

    /** Compare this token with the kind-of-token.
     * 
     * @param k The kind of token to compare with.
     * @return true if the token is of the given kind, false otherwise.
     */
    [[nodiscard]] constexpr bool operator==(kind_type k) const noexcept
    {
        return kind == k;
    }

    /** Compare a simple token with a character. 
     * 
     * This is used to check if a token is a specific single character token.
     * 
     * @param c The character to compare with.
     * @return true if the token is a simple token with the given character, false otherwise
     */
    [[nodiscard]] constexpr bool operator==(char c) const noexcept
    {
        if (kind != kind_type::simple) {
            return false;
        }
        assert(text.size() == 1);
        return text[0] == c;
    }

    /** Compare a identifier or operator token with a string.
     * 
     * This is used to check if a token is a specific identifier or operator.
     * 
     * @param str The string to compare with.
     * @return true if the token is an identifier or operator with the given string, false
     */
    [[nodiscard]] constexpr bool operator==(std::string_view str) const noexcept
    {
        if (kind != kind_type::identifier and kind != kind_type::_operator) {
            return false;
        }
        return text == str;
    }

    /** Append a ASCII character to the token's text.
     * 
     * @param c The character to append.
     */
    constexpr void append(char c)
    {
        assert(static_cast<uint8_t>(c) <= 0x7f);
        text.push_back(c);
    }

    /** Append a code-point to the token's text.
     * 
     * This function encodes the code-point to UTF-8 and appends it to the token's text.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function with a code-point that is not valid Unicode.
     * @param c The code-point to append.
     */
    constexpr void append(char32_t c)
    {
        auto optional_text = encode_utf8_code_point(c);
        assert(optional_text.has_value());
        optional_text.append_to(text);
    }

    /** Append a valid UTF-8 string to the token's text.
     * 
     * @param str The string to append.
     */
    constexpr void append(std::string_view str)
    {
        text.append(str);
    }

    /** Convert this token into an error.
     */
    template<typename... Args>
    token &make_error(file_location last, std::format_string<Args...> fmt, Args &&...args)
    {
        this->kind = kind_type::error;
        this->text = std::format(std::move(fmt), std::forward<Args>(args)...);
        this->last = last;
        return *this;
    }

    /** Normalize the text of the token to NFC form.
     * 
     * This is required for identifiers.
     */
    bool normalize() noexcept
    {
        auto normalized_text = normalize_utf8_string(text);
        if (not normalized_text) {
            make_error(last, "Failed to normalize token text '{}'", text);
            return false;
        }

        text = std::move(normalized_text).value();
        return true;
    }

    /** Check the text of the token for security issues.
     * 
     * This is required for identifiers.
     */
    bool security_check() noexcept
    {
        if (auto const check_result = security_check_utf8_string(text); not check_result) {
            switch (check_result.error()) {
            case utf8_security_error::invalid_utf8_sequence:
                make_error(last, "Token security: Invalid UTF-8 sequence in token '{}'", text);
                break;
            case utf8_security_error::string_to_long:
                make_error(last, "Token security: Token is too long");
                break;
            case utf8_security_error::restriction_level:
                make_error(last, "Token security: Restricted character found in token '{}'", text);
                break;
            case utf8_security_error::invisible:
                make_error(last, "Token security: Invisible character found in token '{}'", text);
                break;
            case utf8_security_error::mixed_numbers:
                make_error(last, "Token security: Mixed numbers found in token '{}'", text);
                break;
            case utf8_security_error::hidden_overlay:
                make_error(last, "Token security: Combining character overlays with base character in token '{}'", text);
                break;
            case utf8_security_error::unknown_error:
                make_error(last, "Token security: Unknown error while checking token '{}'", text);
                break;
            }
            return false;
        }
        return true;
    }

    /** Normalize and security check the token.
     * 
     * This is required for identifiers.
     */
    token &normalize_and_security_check()
    {
        if (not normalize()) {
            return *this;
        }
        if (not security_check()) {
            return *this;
        }
        return *this;
    }

    /** Convert an integer token to its integer value.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function if the token is not an integer literal.
     * @note It is UNDEFINED BEHAVIOR to call this function if the integer value is out of range for long long.
     * @return The integer value of the token.
     */
    [[nodiscard]] long long integer_value() const
    {
        assert(kind == kind_type::integer_literal);
        return std::stoll(text);
    }

    /** Convert a version token to its semantic version value.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function if the token is not a version literal.
     * @return The semantic version value of the token. 
     */
    [[nodiscard]] semantic_version version_value() const
    {
        assert(kind == kind_type::version_literal);
        return semantic_version{text};
    }

    /** Get the simple value of the token.
     * 
     * This is used for single character tokens.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function if the token is not a simple token.
     * @return The single character value of the token.
     */
    [[nodiscard]] char simple_value() const
    {
        assert(kind == kind_type::simple);
        assert(text.size() == 1);
        return text[0];
    }
};

} // namespace hl
