

#include "tokenizer.hpp"


namespace hl {

[[nodiscard]] std::expected<token, std::string> tokenizer::parse_numbered_argument()
{
    assert(_lookahead[0].cp == '$');
    assert(is_digit(_lookahead[1].cp));

    advance(); // Skip the '$' character.
    auto const value_start = _lookahead[0].start;

    auto r = make_token(token::numbered_argument);

    while (decode_utf8()) {
        if (not is_digit(_lookahead[0].cp)) {
            // End of numbered argument.
            r.text = std::string_view{value_start, _lookahead[0].start};
            return r;
        }

        advance();
    }

    r.text = std::string_view{value_start, _end};
    return r;
}

}
