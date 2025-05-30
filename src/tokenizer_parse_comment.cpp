
#include "tokenizer.hpp"
#include "utf8.hpp"
#include <format>
#include <cassert>
#include <utility>

namespace hl {

[[nodiscard]] maybe_expected<token, std::string> tokenizer::parse_line_comment()
{
    enum class state_type {
        start,
        documentation,
        whitespace,
        text
    };

    auto r = make_token(token::documentation);

    assert(_lookahead[0].cp == '/');
    assert(_lookahead[1].cp == '/');
    advance();
    advance();

    auto state = state_type::start;
    auto is_documentation = false;
    while (decode_utf8()) {
        auto const cp = _lookahead[0].cp;

        if (is_vertical_space(cp)) {
            // Don't eat the vertical space, so that the tokenizer can insert a semicollon if needed.
            if (is_documentation and not r.text.empty()) {
                return r;
            } else {
                return {};
            }
        }

        switch (state) {
        case state_type::start:
            // Check if comment or documentation.
            if (cp == '/') {
                state = state_type::documentation;
            } else if (is_horizontal_space(cp)) {
                state = state_type::whitespace;
            } else if (is_documentation) {
                r.append(cp);
                state = state_type::text;
            } else {
                state = state_type::text;
            }
            break;

        case state_type::documentation:
            // Check if documentation in front of a token, or behind a token.
            if (cp == '<') {
                r.kind = token::back_documentation;
                state = state_type::whitespace;
            } else if (is_horizontal_space(cp)) {
                state = state_type::whitespace;
            } else if (is_documentation) {
                r.append(cp);
                state = state_type::text;
            } else {
                state = state_type::text;
            }
            break;

        case state_type::whitespace:
            // Strip leading whitespace.
            if (is_horizontal_space(cp)) {
                state = state_type::whitespace;
            } else if (is_documentation) {
                r.append(cp);
                state = state_type::text;
            } else {
                state = state_type::text;
            }
            break;

        case state_type::text:
            if (is_documentation) {
                r.append(cp);
            }
            state = state_type::text;
            break;
        }
    }

    // This is at end of file.
    if (is_documentation) {
        return r;
    } else {
        return {};
    }
}

[[nodiscard]] maybe_expected<token, std::string> tokenizer::parse_block_comment()
{
    enum class state_type {
        start,
        whitespace,
        star,
        star_space,
        text
    };

    auto r = make_token(token::documentation);

    assert(_lookahead[0].cp == '/');
    assert(_lookahead[1].cp == '*');
    advance();
    advance();

    auto is_documentation = false;
    if (_lookahead[2].cp == '*') {
        is_documentation = true;
        advance();
    }

    auto line_start = false;

    auto state = state_type::start;
    while (decode_utf8()) {
        auto const cp = _lookahead[0].cp;
        auto const cp2 = _lookahead[1].cp;
        auto const cp3 = _lookahead[2].cp;

        if (cp == '*' and cp2 == '/') {
            // End of block comment.
            advance();
            advance();
            if (is_documentation) {
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
            if (is_documentation) {
                r.append('\n');
            }
            line_start = true;

        } else  if (is_documentation) {
            r.append(cp);
        }

        advance();
    }

    return r;
}

[[nodiscard]] maybe_expected<token, std::string> tokenizer::parse_comment()
{
    assert(_lookahead[0].cp == '/');
    if (_lookahead[1].cp == '*') {
        return parse_block_comment();
    } else {
        return parse_line_comment();
    }
}

}