

#include "token_parsers.hpp"
#include "char_category.hpp"


namespace hk {

[[nodiscard]] token parse_number(char const*& p)
{
    auto is_number = is_digit(p[0]);
    is_number |= p[0] == '.' and is_digit(p[1]);
    is_number |= (p[0] == '-' or p[0] == '+') and (is_digit(p[1]) or (p[1] == '.' and is_digit(p[2])));
    if (not is_number) {
        return {};
    }

    auto r = token{p, token::integer_literal};

sign:
    if (p[0] == '+' or p[0] == '-') {
        ++p;
    }

radix_prefix:
    auto radix = 10;
    if (p[0] == '0') {
        if (p[1] == 'b' or p[1] == 'B') {
            radix = 2;
            p += 2;

        } else if (p[1] == 'o' or p[1] == 'O') {
            radix = 8;
            p += 2;

        } else if (p[1] == 'd' or p[1] == 'D') {
            radix = 10;
            p += 2;

        } else if (p[1] == 'x' or p[1] == 'X') {
            radix = 16;
            p += 2;
        }
    }

integer_part:
    while (true) {
        if (is_digit(p[0], radix)) {
            ++p;

        } else if (p[0] == '\'') {
            ++p;

        } else if (p[0] == '.') {
            r.set_kind(token::float_literal);
            ++p;
            goto fraction_part;

        } else if (is_exponent_prefix(radix, p[0])) {
            r.set_kind(token::float_literal);
            ++p;
            goto exponent_part;

        } else {
            // End of number, including end of file.
            r.set_last(p);
            return r;
        }
    }

fraction_part:
    if (p[0] == '*' and p[1] == '.') {
        r.set_kind(token::version_literal);
        p += 2;
        goto patch_part;
    }

    while (true) {
        if (is_digit(p[0], radix)) {
            ++p;

        } else if (p[0] == '\'') {
            ++p;

        } else if (p[0] == '.') {
            r.set_kind(token::version_literal);
            ++p;
            goto patch_part;

        } else if (is_exponent_prefix(radix, p[0])) {
            ++p;
            goto exponent_part;

        } else {
            // End of number, including end of file.
            r.set_last(p);
            return r;
        }
    }

exponent_part:
    // Parse optional sign of the exponent.
    if (p[0] == '+' or p[0] == '-') {
        ++p;
    }

    if (not is_digit(p[0], radix)) {
        return r.make_error(p, token::invalid_exponent_error);
    }

    while (true) {
        if (is_digit(p[0], radix) or p[0] == '\'') {
            ++p;

        } else {
            // End of number, including end of file.
            r.set_last(p);
            return r;
        }
    }

patch_part:
    if (p[0] != '*') {
        ++p;
        r.set_last(p);
        return r;
    }

    while (true) {
        if (is_digit(p[0], radix)) {
            ++p;

        } else if (p[0] == '\'') {
            ++p;

        } else {
            // End of number, including end of file.
            r.set_last(p);
            return r;
        }
    }
}

}
