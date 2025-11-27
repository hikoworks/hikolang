
#include "token_parsers.hpp"
#include "char_category.hpp"
#include <format>
#include <cassert>
#include <utility>

namespace hk {

[[nodiscard]] token parse_block_comment(char const*& p)
{
    auto r = token{};

    if (p[0] != '/' or p[1] != '*') {
        return {};
    }
    r = token{p += 2, token::comment};

    if (p[0] == '*') {
        r = token{++p, token::documentation};
    }

    for (; p[0] != '\0'; ++p) {
        if (p[0] == '*' and p[1] == '/') {
            // End of block comment.
            r.set_last(p);
            p += 2;
            return r;
        }
    }

    return r.make_error(p, token::missing_end_of_block_comment_error);
}

}