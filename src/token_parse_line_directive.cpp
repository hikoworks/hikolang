

#include "token_parsers.hpp"


namespace hl {

[[nodiscard]] std::optional<token> parse_line_directive(file_cursor& c)
{
    auto r = token(c.location(), token::line_feed);
    
    return r;
}

}
