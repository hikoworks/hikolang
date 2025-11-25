
#include "token_parsers.hpp"
#include "char_category.hpp"
#include <gsl/gsl>
#include <cassert>

namespace hk {

[[nodiscard]] token parse_bracketed_string(char const*& p, char open_bracket)
{
    auto const close_bracket = gsl::narrow_cast<char>(mirror_bracket(open_bracket));

    if (open_bracket == '\0') {
        return {};
    }
    assert(open_bracket == '{' or open_bracket == '[' or open_bracket == '(');

    if (p[0] != open_bracket) {
        return {};
    }
    // Consume the opening bracket. Do not include it in the token.
    auto r = token{++p, token::bracketed_string_literal};

    auto bracket_stack = std::vector<char>{};
    auto string_quote = '\0';
    auto backslash = false;
    for (; p[0] != '\0'; ++p) {
        if (string_quote != '\0') {
            if (backslash) {
                backslash = false;
            } else if (p[0] == '\\') {
                backslash = true;
            } else if (p[0] == string_quote) {
                string_quote = '\0';
            }

        } else if (p[0] == '"' or p[0] == '\'' or p[0] == '`') {
            string_quote = p[0];

        } else if (p[0] == '{' or p[0] == '[' or p[0] == '(') {
            bracket_stack.push_back(mirror_bracket(p[0]));

        } else if (p[0] == '}' or p[0] == ']' or p[0] == ')') {
            if (not bracket_stack.empty()) {
                if (p[0] != bracket_stack.back()) {
                    return r.make_error(p, token::unmatched_closing_bracket_error);
                }

                bracket_stack.pop_back();

            } else if (p[0] == close_bracket) {
                // Don't add the closing bracket to the token, just return it.
                r.set_last(p);
                // Consume the closing bracket.
                ++p;
                return r;

            } else {
                return r.make_error(p, token::unmatched_closing_bracket_error);
            }
        }
    }

    return r.make_error(p, token::missing_closing_bracket_error);
}

}
