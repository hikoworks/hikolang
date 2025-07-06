
#pragma once

#include "token.hpp"
#include "utility/file_cursor.hpp"
#include <cstddef>
#include <optional>

namespace hk {

/** Parse an assembly block in the source code.
 * 
 * An assembly block starts at a open brace `{` and ends at a matching close brace `}`.
 * The text between braces must have balanced brackets, brackets inside strings are ignored.
 * 
 *  @param c The file_cursor to read from.
 *  @param enable If true, assembly parsing is enabled, otherwise it is disabled.
 */
[[nodiscard]] std::optional<token> parse_bracketed_string(file_cursor& c, char open_bracket);

[[nodiscard]] std::optional<token> parse_block_comment(file_cursor& c);
[[nodiscard]] std::optional<token> parse_identifier(file_cursor& c);
[[nodiscard]] std::optional<token> parse_line_comment(file_cursor& c);
[[nodiscard]] std::optional<token> parse_line_directive(file_cursor& c);
[[nodiscard]] std::optional<token> parse_number(file_cursor& c);
[[nodiscard]] std::optional<token> parse_operator(file_cursor& c);
[[nodiscard]] std::optional<token> parse_positional_argument(file_cursor& c);
[[nodiscard]] std::optional<token> parse_scram_directive(file_cursor& c);
[[nodiscard]] std::optional<token> parse_string(file_cursor& c);


}
