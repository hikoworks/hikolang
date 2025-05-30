

#include "tokenizer.hpp"


namespace hl {

[[nodiscard]] std::expected<token, std::string> tokenizer::parse_number()
{
    enum class state_type { start, found_sign, zero_prefix, integer_part, fraction_part, found_e, found_esign, exponent_part };

    auto state = state_type::start;

    auto r = make_token(token::integer_literal);
    auto const start_ptr = _lookahead[0].start;
    uint8_t radix = 10;

    auto is_radix_digit = [&](char32_t cp) -> bool {
        if (radix == 2) {
            return cp == '0' or cp == '1';
        } else if (radix == 8) {
            return cp >= '0' and cp <= '7';
        } else if (radix == 10) {
            return cp >= '0' and cp <= '9';
        } else if (radix == 16) {
            return (cp >= '0' and cp <= '9') or (cp >= 'a' and cp <= 'f') or (cp >= 'A' and cp <= 'F');
        }
        return false;
    };

    auto is_exponent_prefix = [&](char32_t cp) -> bool {
        if (radix == 16) {
            return cp == 'p' or cp == 'P';
        } else {
            return cp == 'e' or cp == 'E';
        }
    };

    while (decode_utf8()) {
        auto const cp = _lookahead[0].cp;
        auto const cp_ptr = _lookahead[0].start;

        switch (state) {
        case state_type::start:
            if (cp == '+' or cp == '-') {
                state = state_type::found_sign;
            } else if (cp == '0') {
                state = state_type::zero_prefix;
            } else if (is_radix_digit(cp)) {
                state = state_type::integer_part;
            } else if (cp == '.') {
                r.kind = token::float_literal;
                state = state_type::fraction_part;
            } else {
                return make_unexpected_character_error(cp, cp_ptr);
            }
            break;

        case state_type::found_sign:
            if (cp == '0') {
                state = state_type::zero_prefix;
            } else if (is_radix_digit(cp)) {
                state = state_type::integer_part;
            } else if (cp == '.') {
                r.kind = token::float_literal;
                state = state_type::fraction_part;
            } else {
                return make_unexpected_character_error(cp, cp_ptr);
            }
            break;

        case state_type::zero_prefix:
            if (cp == 'b' or cp == 'B') {
                radix = 2;
                state = state_type::integer_part;
            } else if (cp == 'o' or cp == 'O') {
                radix = 8;
                state = state_type::integer_part;
            } else if (cp == 'd' or cp == 'D') {
                radix = 10;
                state = state_type::integer_part;
            } else if (cp == 'x' or cp == 'X') {
                radix = 16;
                state = state_type::integer_part;
            } else if (is_radix_digit(cp) or cp == '\'') {
                state = state_type::integer_part;
            } else if (cp == '.') {
                r.kind = token::float_literal;
                state = state_type::fraction_part;
            } else if (is_exponent_prefix(cp)) {
                r.kind = token::float_literal;
                state = state_type::found_e;
            } else {
                r.text = std::string_view{start_ptr, cp_ptr};
                return r;
            }
            break;

        case state_type::integer_part:
            if (is_radix_digit(cp) or cp == '\'') {
                state = state_type::integer_part;
            } else if (cp == '.') {
                r.kind = token::float_literal;
                state = state_type::fraction_part;
            } else if (is_exponent_prefix(cp)) {
                r.kind = token::float_literal;
                state = state_type::found_e;
            } else {
                r.text = std::string_view{start_ptr, cp_ptr};
                return r;
            }
            break;

        case state_type::fraction_part:
            if (is_radix_digit(cp) or cp == '\'') {
                state = state_type::fraction_part;
            } else if (is_exponent_prefix(cp)) {
                state = state_type::found_e;
            } else {
                r.text = std::string_view{start_ptr, cp_ptr};
                return r;
            }
            break;

        case state_type::found_e:
            if (cp == '+' or cp == '-') {
                state = state_type::found_esign;
            } else if (is_radix_digit(cp)) {
                state = state_type::exponent_part;
            } else {
                return make_unexpected_character_error(cp, cp_ptr);
            }
            break;

        case state_type::found_esign:
            if (is_radix_digit(cp)) {
                state = state_type::exponent_part;
            } else {
                return make_unexpected_character_error(cp, cp_ptr);
            }
            break;

        case state_type::exponent_part:
            if (is_radix_digit(cp) or cp == '\'') {
                state = state_type::exponent_part;
            } else {
                r.text = std::string_view{start_ptr, cp_ptr};
                return r;
            }
            break;
        }

        advance();
    }

    // If we reach here, we have reached the end of the input.

    // parse_number must only be called if there is at least a single digit.
    assert(state != state_type::start);
    assert(state != state_type::found_sign);

    if (state == state_type::found_e) {
        return make_error("Unterminated number; expected an exponent after 'e' or 'E'.");
    }
    if (state == state_type::found_esign) {
        return make_error("Unterminated number; expected a digit after the sign of an after an 'e' or 'E'.");
    }

    r.text = std::string_view{start_ptr, _end};
    return r;
}

}
