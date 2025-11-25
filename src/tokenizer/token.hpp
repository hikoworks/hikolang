
#pragma once

#include "file_location.hpp"
#include "utility/unicode.hpp"
#include "utility/semantic_version.hpp"
#include <gsl/gsl>
#include <string>
#include <string_view>
#include <cstddef>
#include <cassert>
#include <variant>
#include <format>

namespace hk {

class token {
public:
    enum class kind_type : uint8_t {
        empty,
        missing_end_of_block_comment_error,
        unmatched_closing_bracket_error,
        missing_closing_bracket_error,
        line_directive_eof_error,
        invalid_exponent_error,
        unterminated_string_literal_error,
        invalid_replacement_character_error,
        invalid_surrogate_code_point_error,
        invalid_code_point_error,
        unexpected_character_error,
        unexpected_end_of_comment_error,
        missing_open_bracket_error,
        simple,
        identifier,
        tag,
        context_arg,
        position_arg,
        _operator,
        comment,
        documentation,
        back_documentation,
        string_literal,
        character_literal,
        quote_literal,
        raw_string_literal,
        raw_character_literal,
        raw_quote_literal,
        integer_literal,
        superscript_integer_literal,
        float_literal,
        version_literal,
        line_directive,
        scram_directive,
        bracketed_string_literal,
    };

    static constexpr kind_type empty = kind_type::empty;
    static constexpr kind_type missing_end_of_block_comment_error = kind_type::missing_end_of_block_comment_error;
    static constexpr kind_type unmatched_closing_bracket_error = kind_type::unmatched_closing_bracket_error;
    static constexpr kind_type missing_closing_bracket_error = kind_type::missing_closing_bracket_error;
    static constexpr kind_type line_directive_eof_error = kind_type::line_directive_eof_error;
    static constexpr kind_type invalid_exponent_error = kind_type::invalid_exponent_error;
    static constexpr kind_type unterminated_string_literal_error = kind_type::unterminated_string_literal_error;
    static constexpr kind_type invalid_replacement_character_error = kind_type::invalid_replacement_character_error;
    static constexpr kind_type invalid_surrogate_code_point_error = kind_type::invalid_surrogate_code_point_error;
    static constexpr kind_type invalid_code_point_error = kind_type::invalid_code_point_error;
    static constexpr kind_type unexpected_character_error = kind_type::unexpected_character_error;
    static constexpr kind_type unexpected_end_of_comment_error = kind_type::unexpected_end_of_comment_error;
    static constexpr kind_type missing_open_bracket_error = kind_type::missing_open_bracket_error;
    static constexpr kind_type simple = kind_type::simple;
    static constexpr kind_type identifier = kind_type::identifier;
    static constexpr kind_type tag = kind_type::tag;
    static constexpr kind_type context_arg = kind_type::context_arg;
    static constexpr kind_type position_arg = kind_type::position_arg;
    static constexpr kind_type _operator = kind_type::_operator;
    static constexpr kind_type comment = kind_type::comment;
    static constexpr kind_type documentation = kind_type::documentation;
    static constexpr kind_type back_documentation = kind_type::back_documentation;
    static constexpr kind_type string_literal = kind_type::string_literal;
    static constexpr kind_type character_literal = kind_type::character_literal;
    static constexpr kind_type quote_literal = kind_type::quote_literal;
    static constexpr kind_type raw_string_literal = kind_type::raw_string_literal;
    static constexpr kind_type raw_character_literal = kind_type::raw_character_literal;
    static constexpr kind_type raw_quote_literal = kind_type::raw_quote_literal;
    static constexpr kind_type integer_literal = kind_type::integer_literal;
    static constexpr kind_type superscript_integer_literal = kind_type::superscript_integer_literal;
    static constexpr kind_type float_literal = kind_type::float_literal;
    static constexpr kind_type version_literal = kind_type::version_literal;
    static constexpr kind_type line_directive = kind_type::line_directive;
    static constexpr kind_type scram_directive = kind_type::scram_directive;
    static constexpr kind_type bracketed_string_literal = kind_type::bracketed_string_literal;

    /** The location in the text where the first character of a token is located.
     */
    char const *_ptr = nullptr;

    /** One position after the location of the last character of the token.
     * 
     * When kind() == kind_type::simple, this is the ASCII code-point that
     * represents this token. The size() will be 1.
     */
    uint32_t _size = 0;

    /** The kind of token.
     */
    kind_type _kind = kind_type::empty;

    /** Create a token with the first character's location.
     * 
     * This is used for tokens when the kind is not known yet.
     *
     * @param ptr The location of the first character of the token.
     * @param size The size of the token in number of UTF-8 code-units.
     * @param kind The token kind.
     */
    constexpr token(char const *ptr = nullptr, uint32_t size = 0, kind_type kind = kind_type::empty) noexcept :
        _ptr(ptr), _size(size), _kind(kind)
    {
    }

    /** Create a token with the first character's location and kind.
     * 
     * @param first The location of the first character of the token.
     * @param kind The kind of the token.
     */
    constexpr token(char const *ptr, kind_type kind) noexcept :
        token(ptr, 0, kind)
    {
    }

    /** Create a token with the first character's location and kind.
     * 
     * @param first The location of the first character of the token.
     */
    constexpr token(kind_type kind) noexcept :
        token(nullptr, 0, kind)
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
     * @param ptr The location of the first character of the token.
     * @param c The character of the token.
     * @return A token representing the single character.
     */
    constexpr token(char const *ptr, char c) noexcept :
        token(ptr, c, kind_type::simple)
    {
        assert(static_cast<uint8_t>(c) <= 0x7f);
        assert(c != '\v');
        assert(c != '\f');
        assert(c != '\r');
    }

    constexpr token(token&&) noexcept = default;
    constexpr token(const token&) noexcept = default;
    constexpr token& operator=(token&&) noexcept = default;
    constexpr token& operator=(const token&) noexcept = default;

    [[nodiscard]] constexpr kind_type kind() const noexcept
    {
        return _kind;
    }

    [[nodiscard]] constexpr token& set_kind(kind_type kind) noexcept
    {
        assert(kind != kind_type::simple);
        _kind = kind;
        return *this;
    }

    [[nodiscard]] constexpr token& set_simple(char c) noexcept
    {
        _kind = kind_type::simple;
        _size = c;
        return *this;
    }

    constexpr token& set_first(char const *first) noexcept
    {
        _ptr = first;
        _size = 0;
        return *this;
    }

    constexpr token& set_last(char const *last)
    {
        assert(_kind != kind_type::simple);
        _size = gsl::narrow<uint32_t>(std::distance(_ptr, last));
        return *this;
    }

    constexpr token& set_size(size_t size)
    {
        assert(size == 1 or _kind != kind_type::simple);
        _size = gsl::narrow<uint32_t>(size);
    }

    [[nodiscard]] constexpr size_t size() const noexcept
    {
        if (_kind == kind_type::simple) {
            return 1;
        } else {
            return _size;
        }
    }

    [[nodiscard]] constexpr char const* data() const
    {
        assert(_ptr != nullptr);
        return _ptr;
    }

    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return _kind == kind_type::simple;
    }

    constexpr explicit operator bool() const noexcept
    {
        return not empty();
    }

    constexpr std::string_view string_view() const noexcept
    {
        assert(_kind != kind_type::simple);
        return std::string_view{_ptr, _size};
    }

    constexpr std::string string() const noexcept
    {
        if (_kind == kind_type::simple) {
            return std::string{static_cast<char>(_size)};
        } else {
            return std::string{_ptr, _size};
        }
    }


    /** Compare this token with the kind-of-token.
     * 
     * @param kind The kind of token to compare with.
     * @return true if the token is of the given kind, false otherwise.
     */
    [[nodiscard]] constexpr bool operator==(kind_type kind) const noexcept
    {
        return _kind == kind;
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
        if (_kind != kind_type::simple) {
            return false;
        }
        return static_cast<char>(_size) == c;
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
        if (_kind != kind_type::identifier and _kind != kind_type::_operator) {
            return false;
        }
        return string_view() == str;
    }

    /** Get a NFC normalized UTF-8 string.
     * 
     * This is used for identifiers and operators, so that they can be properly
     * compared. This function also does a security check on the identifier.
     * 
     * @return A NFC normalized UTF-8 string. Or a security error when the
     *         identifier should not be used.
     */
    [[nodiscard]] std::expected<std::string, utf8_security_error> normalized_string_value() const
    {
        auto normalized_text = normalize_utf8_string(string());
        if (not normalized_text) {
            return std::unexpected{utf8_security_error::unknown_error};
        }

        if (auto const check_result = security_check_utf8_string(*normalized_text); not check_result) {
            return std::unexpected{check_result.error()};
        }

        return std::move(normalized_text).value();
    }

    /** Convert a version token to its semantic version value.
     * 
     * @note It is UNDEFINED BEHAVIOR to call this function if the token is not a version literal.
     * @return The semantic version value of the token. 
     */
    [[nodiscard]] semantic_version version_value() const
    {
        assert(_kind == kind_type::version_literal);
        return semantic_version{string_view()};
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
        assert(_kind == kind_type::simple);
        return static_cast<char>(_size);
    }

    [[nodiscard]] token make_error(char const *last, kind_type kind)
    {
        auto tmp = *this;
        tmp.set_last(last);
        tmp.set_kind(kind);
        return tmp;
    }

    [[nodiscard]] token make_error(kind_type kind)
    {
        auto tmp = *this;
        tmp.set_kind(kind);
        return tmp;
    }

};

} // namespace hk
