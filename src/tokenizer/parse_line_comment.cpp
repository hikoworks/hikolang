
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

    for (; p[0] != '\0'; ++p) {
        if (auto const vs_size = is_vertical_space(p)) {
            // Include the vertical space in the token, so that the tokenizer
            // can concatonate sequential line comments.
            r.set_last(p + vs_size);
            // Don't consume the vertical space, so that the tokenizer can
            // see the vertical space for inserting a semicolon.
            return r;
        }
    }

    r.set_last(p);
    return r;
}

}