
#include "token_parsers.hpp"
#include "char_category.hpp"
#include <format>
#include <cassert>
#include <utility>

namespace hk {

[[nodiscard]] token parse_line_comment(char const*& p)
{
    if (p[0] != '/' or p[1] != '/') {
        return {};
    }
    auto r = token{p += 2, token::comment};

    if (p[0] == '/') {
        r = token{++p, token::documentation};
    }

    if (p[0] == '<') {
        r = token{++p, token::back_documentation};
    }

    for (; p[0] != '\0'; ++p) {
        if (is_vertical_space(p)) {
            // Don't consume the vertical space, so that the tokenizer can insert a semicolon if needed.
            r.set_last(p);
            return r;
        }
    }

    r.set_last(p);
    return r;
}

}