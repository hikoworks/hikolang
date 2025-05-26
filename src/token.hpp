
#ifndef HL_TOKEN_HPP
#define HL_TOKEN_HPP

#include "utf8.hpp"
#include <string>
#include <string_view>
#include <cstddef>
#include <cassert>

namespace hl {

class token {
public:
    enum class kind_type {
        end,
        identifier,
        _operator,
        bracket,
        seperator,
        line_feed,
        comment,
        string_literal,
        integer_literal,
        float_literal,
    };

    static constexpr kind_type end = kind_type::end;
    static constexpr kind_type identifier = kind_type::identifier;
    static constexpr kind_type _operator = kind_type::_operator;
    static constexpr kind_type bracket = kind_type::bracket;
    static constexpr kind_type seperator = kind_type::seperator;
    static constexpr kind_type line_feed = kind_type::line_feed;
    static constexpr kind_type comment = kind_type::comment;
    static constexpr kind_type string_literal = kind_type::string_literal;
    static constexpr kind_type integer_literal = kind_type::integer_literal;
    static constexpr kind_type float_literal = kind_type::float_literal;

    std::size_t module_id = 0;
    std::size_t line_nr = 0;
    std::size_t column_nr = 0;
    kind_type kind = kind_type::end;

    std::string text = {};

    constexpr token() noexcept = default;
    constexpr token(std::size_t module_id, std::size_t line_nr, std::size_t column_nr, kind_type kind, std::string text = std::string{}) noexcept :
        module_id(module_id), line_nr(line_nr), column_nr(column_nr), kind(kind), text(std::move(text))
    {
    }

    constexpr token(std::size_t module_id, std::size_t line_nr, std::size_t column_nr, kind_type kind, char c) noexcept :
        module_id(module_id), line_nr(line_nr), column_nr(column_nr), kind(kind), text(1, c)
    {
    }

    constexpr token(std::size_t module_id, std::size_t line_nr, std::size_t column_nr, kind_type kind, char32_t c) noexcept :
        module_id(module_id), line_nr(line_nr), column_nr(column_nr), kind(kind)
    {
        auto optional_text = encode_utf8_code_point(c);
        assert(optional_text.has_value());
        text = *optional_text;
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

private:
    [[nodiscard]] constexpr bool can_simple_compare() const noexcept
    {
        return kind == kind_type::identifier or kind == kind_type::_operator or kind == kind_type::bracket or
            kind == kind_type::seperator or kind == kind_type::line_feed;
    }
};

} // namespace hl

#endif // HL_TOKEN_HPP
