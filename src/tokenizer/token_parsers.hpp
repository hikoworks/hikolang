
#pragma once

#include "token.hpp"
#include "utility/file_cursor.hpp"

namespace hl {

[[nodiscard]] std::optional<token> parse_block_comment(file_cursor& c);
[[nodiscard]] std::optional<token> parse_identifier(file_cursor& c);
[[nodiscard]] std::optional<token> parse_line_comment(file_cursor& c);
[[nodiscard]] std::optional<token> parse_line_directive(file_cursor& c);
[[nodiscard]] std::optional<token> parse_number(file_cursor& c);
[[nodiscard]] std::optional<token> parse_operator(file_cursor& c);
[[nodiscard]] std::optional<token> parse_positional_argument(file_cursor& c);
[[nodiscard]] std::optional<token> parse_string(file_cursor& c);


}
