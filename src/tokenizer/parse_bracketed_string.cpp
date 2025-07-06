
#include "token_parsers.hpp"
#include "utility/char_category.hpp"
#include <gsl/gsl>
#include <cassert>

namespace hk {

[[nodiscard]] std::optional<token> parse_bracketed_string(file_cursor& c, char open_bracket)
{
    if (open_bracket == '\0') {
        return std::nullopt;
    }
    assert(open_bracket == '{' or open_bracket == '[' or open_bracket == '(');

    if (c[0] != open_bracket) {
        return std::nullopt;
    }
    // Consume the opening bracket. Do not include it in the token.
    ++c;

    auto const close_bracket = gsl::narrow_cast<char>(mirror_bracket(open_bracket));

    auto t = token{c.location(), token::bracketed_string_literal};

    auto bracket_stack = std::vector<char>{};
    auto string_quote = '\0';
    auto backslash = false;
    while (not c.end_of_file()) {
        if (string_quote != '\0') {
            if (backslash) {
                backslash = false;
            } else if (c[0] == '\\') {
                backslash = true;
            } else if (c[0] == string_quote) {
                string_quote = '\0';
            }

        } else if (c[0] == '"' or c[0] == '\'' or c[0] == '`') {
            string_quote = c[0];

        } else if (c[0] == '{' or c[0] == '[' or c[0] == '(') {
            bracket_stack.push_back(mirror_bracket(c[0]));

        } else if (c[0] == '}' or c[0] == ']' or c[0] == ')') {
            if (not bracket_stack.empty()) {
                if (c[0] != bracket_stack.back()) {
                    return t.make_error(
                        c.location(), "Unmatched closing bracket found {}; expected '{}'.", gsl::narrow_cast<char>(c[0]), bracket_stack.back());
                }

                bracket_stack.pop_back();

            } else if (c[0] == close_bracket) {
                // Don't add the closing bracket to the token, just return it.
                return t;

            } else {
                return t.make_error(c.location(), "Unmatched closing bracket found '{}', expected '{}'.", gsl::narrow_cast<char>(c[0]), close_bracket);
            }
        }

        t.append(c[0]);
        ++c;
    }

    return t.make_error(c.location(), "Unexpected end of file; expected closing bracket '{}'.", close_bracket);
}

}
