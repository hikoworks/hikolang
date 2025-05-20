
#include <string>
#include <string_view>
#include <cstddef>


namespace hl {

class token {
public:
    enum kind_type {
        end,
        identifier,
        _operator,
        bracket,
        seperator,
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
    static constexpr kind_type comment = kind_type::comment;
    static constexpr kind_type string_literal = kind_type::string_literal;
    static constexpr kind_type integer_literal = kind_type::integer_literal;
    static constexpr kind_type float_literal = kind_type::float_literal;

    kind_type kind = kind_type::end;

    std::size_t line_nr = 0;
    std::size_t column_nr = 0;
    std::size_t module_id = 0;

    std::string text = {};
    std::string comment = {};

    constexpr token() noexcept = default;
    constexpr token(kind_type kind, std::size_t line_nr, std::size_t column_nr,
                    std::size_t module_id, char c) noexcept
        : kind(kind), line_nr(line_nr), column_nr(column_nr),
          module_id(module_id), text(c, 1), comment() {}

        
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
        return
            kind == kind_type::identifier or kind == kind_type::_operator or
            kind == kind_type::bracket or kind == kind_type::seperator;
    }
    
};

[[nodiscard]] constexpr token parse_token(
    size_t &line_nr,
    size_t &column_nr,
    size_t module_id,
    std::string_view::const_iterator& it,
    std::string_view::const_iterator end)
{
    while (it != end) {
        auto const c = *it;
        switch (c) {
        case ' ':
            ++column_nr;
            ++it;
            break;

        case '\t':
            column_nr += 4;
            ++it;
            break;

        case '\n':
            ++line_nr;
            column_nr = 0;
            ++it;
            break;

        case '\r':
            ++it;
            break;

        case '(':
        case ')':
        case '{':
        case '}':
        case '[':
        case ']':
            ++column_nr;
            ++it;
            return token{token::bracket, line_nr, column_nr, module_id, c};

        case ';':
        case ',':
            ++column_nr;
            ++it;
            return token{token::seperator, line_nr, column_nr, module_id, c};

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return parse_literal_integer(line_nr, column_nr, module_id, it, end);

        case '"':
        case '\'':
            return parse_literal_string(line_nr, column_nr, module_id, it, end);
        }
    }

    return token{};
}

}
