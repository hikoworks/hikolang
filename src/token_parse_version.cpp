
#include "token_parsers.hpp"

namespace hl {

[[nodiscard]] std::optional<token> parse_version(file_cursor& c)
{
    if (not is_digit(c[0])) {
        // Versions start with `1` to `_lookahead.size() - 1` digits.
        return std::nullopt;
    }

    for (auto i = 1; i < c.size(); ++i) {
        if (c[i] == 'v') {
            goto major_version;

        } else if (not is_digit(c[i])) {
            return std::nullopt;
        }
    }
    return std::nullopt;
    
major_version:
    auto r = token{c.location(), token::version_literal};

    while (true) {
        if (c[0] == 'v')  {
            r.append('.');
            ++c;
            goto minor_version;

        } else if (is_digit(c[0])) {
            r.append(c[0]);
            ++c;

        } else {
            // It was already proven that the major version was complete.
            std::unreachable();
        }
    }

minor_version:
    while (true) {
        if (c[0] == '.') {
            r.append('.');
            ++c;
            goto patch_version;

        } else if (is_digit(c[0])) {
            r.append(c[0]);
            ++c;

        } else {
            // End of version, including end of file.
            r.last = c.location();
            return r;
        }
    }

patch_version:
    while (true) {
        if (is_digit(c[0])) {
            r.append(c[0]);
            ++c;

        } else {
            // End of version, including end of file.
            r.last = c.location();
            return r;
        }
    }
}

} // namespace hl