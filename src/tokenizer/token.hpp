
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
        end_of_file,
        error,
        identifier,
        _operator,
        bracket,
        seperator,
        line_feed,
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
    };

    static constexpr kind_type empty = kind_type::empty;
    static constexpr kind_type end_of_file = kind_type::end_of_file;
    static constexpr kind_type error = kind_type::error;
    static constexpr kind_type identifier = kind_type::identifier;
    static constexpr kind_type _operator = kind_type::_operator;
    static constexpr kind_type bracket = kind_type::bracket;
    static constexpr kind_type seperator = kind_type::seperator;
    static constexpr kind_type line_feed = kind_type::line_feed;
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

    constexpr token() noexcept = default;

    constexpr token(file_location const &first) noexcept :
        first(first), last(first), kind(kind_type::empty)
    {
    }

    constexpr token(file_location const &first, kind_type kind) noexcept :
        first(first), last(first), kind(kind)
    {
    }

    /** Construct a token with a single character.
     * 
     * The character must be a valid ASCII character (0x00 to 0x7f).
     * The `last` location will be advanced to the next position after the character.
     * 
     * This is used for single-character tokens like brackets, separators, and operators.
     * 
     * @param first The location of the first character of the token.
     * @param kind The kind of the token.
     * @param c The character of the token.
     */
    constexpr token(file_location const &first, kind_type kind, char c) noexcept :
        first(first), last(first), kind(kind), text(1, c)
    {
        // Ensure that the character is a valid ASCII character.
        assert(static_cast<uint8_t>(c) <= 0x7f);
        // A token can not be a carriage return, it will properly be handled by advance.
        assert(c != '\r');
        last.advance(c, '\0');
    }

    /** Construct a token with a single character.
     * 
     * The `last` location will be advanced to the next position after the character.
     * 
     * This is used for single-character tokens like brackets, separators, and operators.
     * 
     * @param first The location of the first character of the token.
     * @param kind The kind of the token.
     * @param c The character of the token.
     */
    constexpr token(file_location const &first, kind_type kind, char32_t c) noexcept :
        first(first), last(first), kind(kind), text()
    {
        // A token can not be a carriage return, it will properly be handled by advance.
        assert(c != '\r');
        last.advance(c, '\0');
        
        auto optional_text = encode_utf8_code_point(c);
        assert(optional_text.has_value());
        optional_text.append_to(text);
    }

    constexpr token(token&&) noexcept = default;
    constexpr token(const token&) noexcept = default;
    constexpr token& operator=(token&&) noexcept = default;
    constexpr token& operator=(const token&) noexcept = default;

    [[nodiscard]] constexpr bool operator==(char c) const noexcept
    {
        return can_simple_compare() and text.size() == 1 and text[0] == c;
    }

    [[nodiscard]] constexpr bool operator==(std::string_view str) const noexcept
    {
        return can_simple_compare() and text == str;
    }

    constexpr void append(char c)
    {
        assert(static_cast<uint8_t>(c) <= 0x7f);
        text.push_back(c);
    }

    constexpr void append(char32_t c)
    {
        auto optional_text = encode_utf8_code_point(c);
        assert(optional_text.has_value());
        optional_text.append_to(text);
    }

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

    [[nodiscard]] constexpr bool has_value() const noexcept
    {
        return kind != kind_type::empty and not text.empty();
    }

    [[nodiscard]] long long integer_value() const noexcept
    {
        assert(kind == kind_type::integer_literal);
        return std::stoll(text);
    }

    [[nodiscard]] semantic_version version_value() const noexcept
    {
        assert(kind == kind_type::version_literal);
        return semantic_version{text};
    }

    

    

private:
    [[nodiscard]] constexpr bool can_simple_compare() const noexcept
    {
        return kind == kind_type::identifier or kind == kind_type::_operator or kind == kind_type::bracket or
            kind == kind_type::seperator or kind == kind_type::line_feed;
    }
};

} // namespace hl
