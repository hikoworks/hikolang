
#include "tokenizer.hpp"
#include "utf8.hpp"
#include <format>
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <string>
#include <string_view>
#include <expected>
#include <utility>

namespace hl {

[[nodiscard]] std::expected<token, std::string> tokenizer::parse_operator()
{
    auto r = make_token(token::_operator);
    auto const start_ptr = _lookahead[0].start;

    // Skip the first character, which is guaranteed to be a pattern syntax character.
    advance();

    while (decode_utf8()) {
        if (not is_pattern_syntax(_lookahead[0].cp)) {
            // End of operator.
            r.text = std::string_view{start_ptr, _lookahead[0].start};
            return r;
        }

        advance();
    }

    r.text = std::string_view{start_ptr, _end};
    return r;
}

} // namespace hl
