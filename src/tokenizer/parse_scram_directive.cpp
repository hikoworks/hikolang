

#include "token_parsers.hpp"

namespace hl {

[[nodiscard]] std::optional<token> parse_scram_directive(file_cursor& c)
{
    if (c.location().column != 0) {
        // The line directive must be at the start of the line.
        return std::nullopt;
    }

    if (c[0] != '#' or c[1] != 's' or c[2] != 'c' or c[3] != 'r' or c[4] != 'a' or c[5] != 'm' or c[6] != ' ' or not is_digit(c[7])) {
        return std::nullopt;
    }

    auto r = token{c.location(), token::line_directive};

    // Skip the "#scram " part.
    c += 7;

    std::uint32_t key = 0;

    if (auto n = parse_number(c)) {
        if (n->kind != token::integer_literal) {
            return r.make_error(c.location(), "Invalid #scarm directive; expected a integer key, got '{}'.", n->text);
        }

        key = std::stoull(n->text);
        if (key == 0) {
            return r.make_error(c.location(), "Invalid #scram directive; key must not be zero.");
        }

    } else {
        return r.make_error(c.location(), "Invalid #scram directive; expected a integer key.");
    }

    while (not is_vertical_space(c[0], c[1])) {
        if (is_horizontal_space(c[0], c[1])) {
            ++c;
        } else {
            // Invalid character, or end of file.
            return r.make_error(c.location(), "Invalid #scram directive; expected a line ending after the key.");        
        }
    }

    c.set_scram_key(key);
    return r;
}

}
