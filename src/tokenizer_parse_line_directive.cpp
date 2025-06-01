

#include "tokenizer.hpp"


namespace hl {

[[nodiscard]] maybe_expected<token, std::string> tokenizer::parse_line_directive()
{
    auto r = make_token(token::line_feed);
    
    return r;
}

}
