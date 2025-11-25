

#include "token_parsers.hpp"
#include "char_category.hpp"

namespace hk {

[[nodiscard]] token parse_line_directive(char const*& p)
{
    if (p[0] != '#' or p[1] != 'l' or p[2] != 'i' or p[3] != 'n' or p[4] != 'e' or p[5] != ' ' or not is_digit(p[6])) {
        return {};
    }

    auto r = token{p += 6, token::line_directive};

    std::size_t line_number = 0;
    std::string file_name = {};

    for (; p[0] != '\0'; ++p) {
        if (auto n = is_vertical_space(p)) {
            r.set_last(p);
            p += n;
            return r;
        }
    }

    return r.make_error(p, token::line_directive_eof_error);
}

}
