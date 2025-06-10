
#include "token_parsers.hpp"
#include "char_category.hpp"
#include "utf8.hpp"
#include <format>
#include <cassert>
#include <utility>

namespace hl {

[[nodiscard]] std::optional<token> parse_block_comment(file_cursor& c)
{
    enum class state_type {
        start,
        whitespace,
        star,
        star_space,
        text
    };

    if (c[0] != '/' or c[1] != '*') {
        return std::nullopt;
    }
    auto r = token{c.location(), token::comment};
    c += 2;

    if (c[0] == '*') {
        r.kind = token::documentation;
        ++c;
    }

    auto line_start = false;

    auto state = state_type::start;
    while (not c.end_of_file()) {
        if (c[0] == '*' and c[1] == '/') {
            // End of block comment.
            c += 2;
            r.last = c.location();
            return r;

        } else if (line_start and c[0] == '*' and is_horizontal_space(c[1])) {
            // Skip the leading '*' and horizontal space after it.
            ++c;
            line_start = false;

        } else if (line_start and c[0] == '*') {
            // Skip the leading '*'.
            line_start = false;

        } else if (line_start and is_horizontal_space(c[0])) {
            // Skip all leading spaces before an optional '*'.

        } else if (is_vertical_space(c[0])) {
            // Don't eat the vertical space, so that the tokenizer can insert a semicollon if needed.
            if (r.kind != token::empty) {
                r.append('\n');
            }
            line_start = true;

        } else if (r.kind != token::empty) {
            r.append(c[0]);
        }

        ++c;
    }

    return r.make_error(c.location(), "Unterminated block comment; expected '*/' at the end of the comment.");
}

}