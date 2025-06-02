

#include "tokenizer.hpp"


namespace hl {

[[nodiscard]] maybe_expected<token, std::string> tokenizer::parse_positional_argument()
{
    auto const is_positional_argument = _lookahead[0].cp == '$' and is_digit(_lookahead[1].cp);
    if (not is_positional_argument) {
        return {};
    }

    advance(); // Skip the '$' character.
    auto const value_start = _lookahead[0].start;

    while (decode_utf8()) {
        if (not is_digit(_lookahead[0].cp)) {
            // End of numbered argument.
            return make_token(token::positional_argument, std::string{value_start, _lookahead[0].start});
        }

        advance();
    }

    return make_token(token::positional_argument, std::string{value_start, _end});
}

}
