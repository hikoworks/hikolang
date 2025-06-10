

#include "token_parsers.hpp"


namespace hl {

[[nodiscard]] static bool is_radix_digit(int radix, char32_t cp) noexcept
{
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
}

[[nodiscard]] static bool is_exponent_prefix(int radix, char32_t cp) noexcept
{
    if (radix == 16) {
        return cp == 'p' or cp == 'P';
    } else {
        return cp == 'e' or cp == 'E';
    }
}

[[nodiscard]] std::optional<token> parse_number(file_cursor& c)
{
    auto is_number = is_digit(c[0]);
    is_number |= c[0] == '.' and is_digit(c[1]);
    is_number |= (c[0] == '-' or c[0] == '+') and (is_digit(c[1]) or (c[1] == '.' and is_digit(c[2])));
    if (not is_number) {
        return std::nullopt;
    }

    auto r = token{c.location(), token::integer_literal};

sign:
    if (c[0] == '+' or c[0] == '-') {
        r.append(c[0]);
        ++c;
    }

radix_prefix:
    auto radix = 10;
    if (c[0] == '0') {
        if (c[1] == 'b' or c[1] == 'B') {
            radix = 2;
            r.append(c[0]);
            r.append(c[1]);
            c += 2;

        } else if (c[1] == 'o' or c[1] == 'O') {
            radix = 8;
            r.append(c[0]);
            r.append(c[1]);
            c += 2;

        } else if (c[1] == 'd' or c[1] == 'D') {
            radix = 10;
            r.append(c[0]);
            r.append(c[1]);
            c += 2;

        } else if (c[1] == 'x' or c[1] == 'X') {
            radix = 16;
            r.append(c[0]);
            r.append(c[1]);
            c += 2;
        }
    }

integer_part:
    while (true) {
        if (is_radix_digit(radix, c[0])) {
            r.append(c[0]);
            ++c;

        } else if (c[0] == '\'') {
            ++c;

        } else if (c[0] == '.') {
            r.kind = token::float_literal;
            r.append(c[0]);
            ++c;
            goto fraction_part;

        } else if (is_exponent_prefix(radix, c[0])) {
            r.kind = token::float_literal;
            r.append(c[0]);
            ++c;
            goto exponent_part;

        } else {
            // End of number, including end of file.
            r.last = c.location();
            return r;
        }
    }

fraction_part:
    if (c[0] == '*' and c[1] == '.') {
        r.kind = token::version_literal;
        r.append(c[0]);
        r.append(c[1]);
        c += 2;
        goto patch_part;
    }

    while (true) {
        if (is_radix_digit(radix, c[0])) {
            r.append(c[0]);
            ++c;

        } else if (c[0] == '\'') {
            ++c;

        } else if (c[0] == '.') {
            r.kind = token::version_literal;
            r.append(c[0]);
            ++c;
            goto patch_part;

        } else if (is_exponent_prefix(radix, c[0])) {
            r.append(c[0]);
            ++c;
            goto exponent_part;

        } else {
            // End of number, including end of file.
            r.last = c.location();
            return r;
        }
    }

exponent_part:
    // Parse optional sign of the exponent.
    if (c[0] == '+' or c[0] == '-') {
        r.append(c[0]);
        ++c;
    }

    if (not is_radix_digit(radix, c[0])) {
        return r.make_error(c.location(), "Unterminated number; expected a digit after 'e' or 'E'.");
    }

    while (true) {
        if (is_radix_digit(radix, c[0]) or c[0] == '\'') {
            r.append(c[0]);
            ++c;

        } else {
            // End of number, including end of file.
            r.last = c.location();
            return r;
        }
    }

patch_part:
    if (c[0] != '*') {
        r.append(c[0]);
        ++c;
        r.last = c.location();
        return r;
    }

    while (true) {
        if (is_radix_digit(radix, c[0])) {
            r.append(c[0]);
            ++c;

        } else if (c[0] == '\'') {
            ++c;

        } else {
            // End of number, including end of file.
            r.last = c.location();
            return r;
        }
    }
}

}
