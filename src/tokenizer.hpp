
#include "token.hpp"
#include <concepts>
#include <cstddef>

namespace hl  {

struct parse_token_context {
    std::size_t line_nr = 0;
    std::size_t column_nr = 0;
    std::size_t module_id = 0;

    std::string text = {};
    std::string bracket_stack = {};

    constexpr void advance(char c) noexcept
    {
        if (c == '\n') {
            ++line_nr;
            column_nr = 0;
        } else if ((c & 0xc0) == 0x80) {
            // Skip UTF-8 continuation code-unit.
        } else {
            ++column_nr;
        }
    }
};

constexpr token parse_token(std::string_view::const_iterator& it,
                            std::string_view::const_iterator end) {
    auto start = it;
    while (it != end && !std::isspace(*it)) {
        ++it;
    }
    return token(start, it);
}

template<std::invocable<hl::token> F>
constexpr void tokenize(std::string_view text, size_t module_id, F&& f) {
    auto context = parse_token_context{};
    context.module_id = module_id;

    auto it = text.begin();
    auto const end = text.end();
    while (it != end) {
        auto const c = *it;
        context.advance(c);

        switch (c) {
        case '{':
        case '[':
        case '(':
            context.bracket_stack.push_back(c);
            f(token{context.module_id, context.line_nr, context.column_nr, token::bracket, c});
            break;
        }
    }
}

}
