

#include "token_parsers.hpp"


namespace hk {

[[nodiscard]] std::optional<token> parse_superscript_integer(file_cursor& c)
{
    auto is_number = is_superscript_digit(c[0]);
    is_number |= (c[0] == U'⁺' or c[0] == U'⁻') and is_superscript_digit(c[1]);
    if (not is_number) {
        return std::nullopt;
    }

    auto r = token{c.location(), token::super_integer_literal};

    if (c[0] == U'⁺') {
        r.append('+');
        ++c;
    } else if (c[0] == U'⁻') {
        r.append('-');
        ++c;
    }

    while (true) {
        if (c[0] == '\'') {
            ++c;

        } else if (c[0] == U'⁰') {
            r.append('0');
            ++c;

        } else if (c[0] == U'¹') {
            r.append('1');
            ++c;

        } else if (c[0] == U'²') {
            r.append('2');
            ++c;

        } else if (c[0] == U'³') {
            r.append('3');
            ++c;

        } else if (c[0] == U'⁴') {
            r.append('4');
            ++c;

        } else if (c[0] == U'⁵') {
            r.append('5');
            ++c;

        } else if (c[0] == U'⁶') {
            r.append('6');
            ++c;

        } else if (c[0] == U'⁷') {
            r.append('7');
            ++c;

        } else if (c[0] == U'⁸') {
            r.append('8');
            ++c;

        } else if (c[0] == U'⁹') {
            r.append('9');
            ++c;

        } else {
            // End of number, including end of file.
            r.last = c.location();
            return r;
        }
    }
}

}
