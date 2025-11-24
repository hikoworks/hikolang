
#include "token_parsers.hpp"
#include "char_category.hpp"
#include <format>
#include <cassert>
#include <utility>

namespace hk {

[[nodiscard]] std::optional<token> parse_block_comment(char const*& p)
{
    enum class state_type {
        start,
        whitespace,
        star,
        star_space,
        text
    };

    auto location = p;

    if (p[0] != '/' or p[1] != '*') {
        return std::nullopt;
    }
    auto r = token{location, token::comment};
    p += 2;

    if (p[0] == '*') {
        r.kind = token::documentation;
        ++p;
    }

    auto line_start = false;
    while (p[0] != '\0') {
        if (p[0] == '*' and p[1] == '/') {
            // End of block comment.
            p += 2;
            r.last = p;
            return r;

        } else if (line_start and p[0] == '*' and is_horizontal_space_advance(p)) {
            // Skip the leading '*' and a single horizontal space after it.
            ++p;
            line_start = false;

        } else if (line_start and p[0] == '*') {
            // Skip the leading '*'.
            line_start = false;

        } else if (line_start and is_horizontal_space_advance(p)) {
            // Skip all leading spaces before an optional '*'.

        } else if (is_vertical_space_advance(p)) {
            // Don't eat the vertical space, so that the tokenizer can insert a semicolon if needed.
            if (r.kind != token::empty) {
                r.append('\n');
            }
            line_start = true;

        } else if (r.kind != token::empty) {
            r.append(p[0]);
        }

        ++p;
    }

    return r.make_error(location, "Unterminated block comment; expected '*/' at the end of the comment.");
}

}