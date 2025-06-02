
#include "tokenizer.hpp"
#include "utf8.hpp"
#include <format>
#include <cassert>
#include <utility>

namespace hl {

[[nodiscard]] maybe_expected<token, std::string> tokenizer::parse_line_comment()
{
    if (_lookahead[0].cp != '/' or _lookahead[1].cp != '/') {
        return {};
    }

    advance();
    advance();

    auto r = make_token();

    if (_lookahead[0].cp == '/') {
        r.kind = token::documentation;
        advance();
    }

    if (_lookahead[0].cp == '<') {
        // This is a documentation comment in front of a token.
        r.kind = token::back_documentation;
        advance();
    }

    auto text_start = _lookahead[0].start;
    while (not end_of_file()) {
        auto const cp = _lookahead[0].cp;

        if (is_vertical_space(cp)) {
            // Don't eat the vertical space, so that the tokenizer can insert a semicolon if needed.
            if (r.kind != token::empty) {
                r.text = std::string{text_start, _lookahead[0].start};
                return r;
            } else {
                return {};
            }
        }
    }

    // This is at end of file.
    if (r.kind != token::empty) {
        r.text = std::string{text_start, _end};
        return r;
    } else {
        return {};
    }
}

}