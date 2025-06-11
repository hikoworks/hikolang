
#include "token_parsers.hpp"
#include <format>
#include <cassert>
#include <utility>

namespace hl {

[[nodiscard]] std::optional<token> parse_line_comment(file_cursor& c)
{
    if (c[0] != '/' or c[1] != '/') {
        return std::nullopt;
    }
    
    auto r = token{c.location(), token::comment};
    c += 2; // Skip the two slashes.

    if (c[0] == '/') {
        r.kind = token::documentation;
        ++c; // Skip the third slash.
    }

    if (c[0] == '<') {
        // This is a documentation comment in front of a token.
        r.kind = token::back_documentation;
        ++c; // Skip the '<' character.
    }

    while (true) {
        if (is_vertical_space(c[0], c[1]) or c.end_of_file()) {
            // Don't eat the vertical space, so that the tokenizer can insert a semicolon if needed.
            r.last = c.location();
            return r;
        }
        r.append(c[0]);
        ++c;
    }
}

}