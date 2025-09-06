

#include "token_parsers.hpp"

namespace hk {

[[nodiscard]] std::optional<token> parse_line_directive(file_cursor& c)
{
    if (c.location().utf8_column != 0) {
        // The line directive must be at the start of the line.
        return std::nullopt;
    }

    if (c[0] != '#' or c[1] != 'l' or c[2] != 'i' or c[3] != 'n' or c[4] != 'e' or c[5] != ' ' or not is_digit(c[6])) {
        return std::nullopt;
    }

    auto r = token{c.location(), token::line_directive};

    // Skip the "#line " part.
    c += 6;

    std::size_t line_number = 0;
    std::string file_name = {};

    if (auto n = parse_number(c)) {
        if (n->kind != token::integer_literal) {
            return r.make_error(c.location(), "Invalid #line directive; expected a integer line number, got '{}'.", n->text);
        }

        line_number = std::stoull(n->text);
        if (line_number == 0) {
            return r.make_error(c.location(), "Invalid #line directive; line number must be greater than zero.");
        }

    } else {
        return r.make_error(c.location(), "Invalid #line directive; expected a line number.");
    }
    
    if (is_horizontal_space(c[0], c[1])) {
        while (is_horizontal_space(c[0], c[1])) {
            ++c;
        }

        if (auto s = parse_string(c)) {
            if (s->kind != token::string_literal) {
                return r.make_error(c.location(), "Invalid #line directive; expected a string literal for the file name, got '{}'.", s->text);
            }

            file_name = std::move(s->text);
        } else {
            return r.make_error(c.location(), "Invalid #line directive; expected a string literal for the file name.");
        }
    }

    while (not is_vertical_space(c[0], c[1])) {
        if (is_horizontal_space(c[0], c[1])) {
            ++c;
        } else {
            // Invalid character, or end of file.
            return r.make_error(c.location(), "Invalid #line directive; expected a line ending after the file name.");        
        }
    }

    // Convert to zero-based line number.
    --line_number; 

    if (file_name.empty()) {
        c.set_line(line_number);
    } else {
        c.set_line(std::filesystem::path{file_name}, line_number);
    }

    return r;
}

}
