
#include "tokenizer.hpp"

namespace hl {

[[nodiscard]] maybe_expected<token, std::string> tokenizer::parse_version()
{
    auto const cp = _lookahead[0].cp;
    if (not is_digit(cp)) {
        // Versions start with `1` to `_lookahead.size() - 1` digits.
        return {};
    }

    auto is_version = false;
    for (auto i = 1; i < _lookahead.size(); ++i) {
        auto const cp = _lookahead[i].cp;
        if (cp == 'v') {
            is_version = true;
            break;
        } else if (not is_digit(cp)) {
            return {};
        }
    }
    if (not is_version) {
        return {};
    }
    
    auto const start_ptr = _lookahead[0].start;

    // Major version must start with digits followed by 'v'.
    while (not end_of_file()) {
        auto const cp = _lookahead[0].cp;
        if (cp == 'v')  {
            advance();
            break;
        }
        assert(is_digit(cp));
        advance();
    }

    // Minor version starts with digits, possibly followed by a '.'.
    while (not end_of_file()) {
        auto const cp = _lookahead[0].cp;
        if (cp == '.') {
            advance();
            break;

        } else if (is_digit(cp)) {
            advance();
            continue;

        } else {
            return make_token(token::version_literal, std::string{start_ptr, _lookahead[0].start});
        }
    }

    // Patch version is just digits.
    while (not end_of_file()) {
        auto const cp = _lookahead[0].cp;
        if (is_digit(cp)) {
            advance();
            continue;

        } else {
            // End of version.
            return make_token(token::version_literal, std::string{start_ptr, _lookahead[0].start});
        }
    }

    return make_token(token::version_literal, std::string{start_ptr, _end});
}

} // namespace hl