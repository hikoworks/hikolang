
#ifndef HL_TOKEN_HPP
#define HL_TOKEN_HPP

#include "utf8.hpp"
#include <string>
#include <string_view>
#include <cstddef>
#include <cassert>
#include <variant>

namespace hl {

class token {
public:

    enum class kind_type {
        empty,
        error,
        identifier,
        _operator,
        bracket,
        seperator,
        line_feed,
        documentation,
        back_documentation,
        string_literal,
        character_literal,
        quote_literal,
        integer_literal,
        float_literal,
        version_literal,
        positional_argument,
    };

    static constexpr kind_type empty = kind_type::empty;
    static constexpr kind_type error = kind_type::error;
    static constexpr kind_type identifier = kind_type::identifier;
    static constexpr kind_type _operator = kind_type::_operator;
    static constexpr kind_type bracket = kind_type::bracket;
    static constexpr kind_type seperator = kind_type::seperator;
    static constexpr kind_type line_feed = kind_type::line_feed;
    static constexpr kind_type documentation = kind_type::documentation;
    static constexpr kind_type back_documentation = kind_type::back_documentation;
    static constexpr kind_type string_literal = kind_type::string_literal;
    static constexpr kind_type character_literal = kind_type::character_literal;
    static constexpr kind_type quote_literal = kind_type::quote_literal;
    static constexpr kind_type integer_literal = kind_type::integer_literal;
    static constexpr kind_type float_literal = kind_type::float_literal;
    static constexpr kind_type version_literal = kind_type::version_literal;
    static constexpr kind_type positional_argument = kind_type::positional_argument;

    /** The file this token belongs to.
     */
    std::size_t file_id = 0;

    /** The byte-index to the first character of the token.
     */
    std::size_t first = 0;

    /** The byte-index to one beyond the last character of the token.
     */
    std::size_t last = 0;

    kind_type kind = kind_type::empty;

    /** The significant part of the token.
     */
    std::string text = {};

    constexpr token() noexcept = default;

    constexpr token(std::size_t file_id, std::size_t first, std::size_t last) noexcept :
        file_id(file_id), first(first), last(last), kind(kind_type::empty)
    {
    }

    constexpr token(std::size_t file_id, std::size_t first, std::size_t last, kind_type kind) noexcept :
        file_id(file_id), first(first), last(last), kind(kind)
    {
    }

    constexpr token(std::size_t file_id, std::size_t first, std::size_t last, kind_type kind, std::string text) noexcept :
        file_id(file_id), first(first), last(last), kind(kind), text(std::move(text))
    {
    }

    constexpr token(std::size_t file_id, std::size_t first, std::size_t last, kind_type kind, char c) noexcept :
        file_id(file_id), first(first), last(last), kind(kind), text(1, c)
    {
    }

    constexpr token(std::size_t file_id, std::size_t first, std::size_t last, kind_type kind, char32_t c) noexcept :
        file_id(file_id), first(first), last(last), kind(kind), text()
    {
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


private:
    [[nodiscard]] constexpr bool can_simple_compare() const noexcept
    {
        return kind == kind_type::identifier or kind == kind_type::_operator or kind == kind_type::bracket or
            kind == kind_type::seperator or kind == kind_type::line_feed;
    }
};

} // namespace hl

#endif // HL_TOKEN_HPP
