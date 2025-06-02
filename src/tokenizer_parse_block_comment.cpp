
#include "tokenizer.hpp"
#include "utf8.hpp"
#include <format>
#include <cassert>
#include <utility>

namespace hl {

[[nodiscard]] maybe_expected<token, std::string> tokenizer::parse_block_comment()
{
    enum class state_type {
        start,
        whitespace,
        star,
        star_space,
        text
    };

    if (_lookahead[0].cp != '/' or _lookahead[1].cp != '*') {
        return {};
    }
    advance();
    advance();

    auto r = make_token();
    if (_lookahead[0].cp == '*') {
        r.kind = token::documentation;
        advance();
    }

    auto line_start = false;

    auto state = state_type::start;
    while (not end_of_file()) {
        auto const cp = _lookahead[0].cp;
        auto const cp2 = _lookahead[1].cp;
        auto const cp3 = _lookahead[2].cp;

        if (cp == '*' and cp2 == '/') {
            // End of block comment.
            advance();
            advance();
            if (r.kind != token::empty) {
                return r;
            } else {
                return {};
            }

        } else if (line_start and cp == '*' and is_horizontal_space(cp2)) {
            // Skip the leading '*' and horizontal space after it.
            advance();
            line_start = false;

        } else if (line_start and cp == '*') {
            // Skip the leading '*'.
            line_start = false;

        } else if (line_start and is_horizontal_space(cp)) {
            // Skip all leading spaces before an optional '*'.

        } else if (is_vertical_space(cp)) {
            // Don't eat the vertical space, so that the tokenizer can insert a semicollon if needed.
            if (r.kind != token::empty) {
                r.append('\n');
            }
            line_start = true;

        } else if (r.kind != token::empty) {
            r.append(cp);
        }

        advance();
    }

    if (r.kind != token::empty) {
        return r;
    } else {
        return {};
    }
}

}