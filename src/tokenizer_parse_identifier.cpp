

#include "tokenizer.hpp"
#include <unicode/normalizer2.h>

namespace hl {

[[nodiscard]] std::expected<token, std::string> tokenizer::parse_identifier()
{
    auto r = make_token(token::identifier);
    auto const start_ptr = _lookahead[0].start;

    // Skip the first character, which is guaranteed to be an identifier start.
    advance();

    auto identifier = [&] {
        while (decode_utf8()) {
            auto const cp = _lookahead[0].cp;

            if (not is_identifier_continue(cp) and cp != '_') {
                // End of identifier.
                return std::string{start_ptr, _lookahead[0].start};
            }

            advance();
        }
        // Identifier continues until the end of the input.
        return std::string{start_ptr, _end};
    }();

    // NFC normalization is required for identifiers to ensure they are in a canonical form.
    // This is done with only moves, if the string is already in NFC form.
    auto normalized_identifier = normalize_utf8_string(identifier);
    if (not normalized_identifier) {
        return make_error("Failed to normalize identifier '{}'", identifier);
    }

    r.text = std::move(normalized_identifier).value();

    if (auto check_result = security_check_utf8_string(r.text); not check_result) [[unlikely]] {
        switch (check_result.error()) {
        case utf8_security_error::invalid_utf8_sequence:
            return make_error("Identifier security: Invalid UTF-8 sequence in identifier '{}'", r.text);
        case utf8_security_error::string_to_long:
            return make_error("Identifier security: Identifier is too long");
        case utf8_security_error::restriction_level:
            return make_error("Identifier security: Restricted character found in identifier '{}'", r.text);
        case utf8_security_error::invisible:
            return make_error("Identifier security: Invisible character found in identifier '{}'", r.text);
        case utf8_security_error::mixed_numbers:
            return make_error("Identifier security: Mixed numbers found in identifier '{}'", r.text);
        case utf8_security_error::hidden_overlay:
            return make_error("Identifier security: Combining character overlays with base character in identifier '{}'", r.text);
        case utf8_security_error::unknown_error:
            return make_error("Identifier security: Unknown error while checking identifier '{}'", r.text);
        }
    }

    return r;
}

}

