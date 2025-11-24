
#include "token_parsers.hpp"
#include "char_category.hpp"
#include <gsl/gsl>
#include <cassert>

namespace hk {

[[nodiscard]] std::optional<token> parse_bracketed_string(char const*& p, char open_bracket)
{
    if (open_bracket == '\0') {
        return std::nullopt;
    }
    assert(open_bracket == '{' or open_bracket == '[' or open_bracket == '(');

    if (p[0] != open_bracket) {
        return std::nullopt;
    }
    // Consume the opening bracket. Do not include it in the token.
    ++p;

    auto const close_bracket = gsl::narrow_cast<char>(mirror_bracket(open_bracket));

    auto location = p;
    auto t = token{location, token::bracketed_string_literal};

    auto bracket_stack = std::vector<char>{};
    auto string_quote = '\0';
    auto backslash = false;
    while (p[0] != '\0') {
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
                    return t.make_error(
                        location, "Unmatched closing bracket found {}; expected '{}'.", gsl::narrow_cast<char>(p[0]), bracket_stack.back());
                }

                bracket_stack.pop_back();

            } else if (p[0] == close_bracket) {
                // Don't add the closing bracket to the token, just return it.
                return t;

            } else {
                return t.make_error(location, "Unmatched closing bracket found '{}', expected '{}'.", gsl::narrow_cast<char>(p[0]), close_bracket);
            }
        }

        t.append(p[0]);
        ++p;
    }

    return t.make_error(location, "Unexpected end of file; expected closing bracket '{}'.", close_bracket);
}

}
