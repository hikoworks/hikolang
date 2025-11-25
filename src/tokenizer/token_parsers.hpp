
#pragma once

#include "token.hpp"
#include "file_cursor.hpp"
#include <cstddef>
#include <optional>

namespace hk {

/** Parse an assembly block in the source code.
 * 
 * An assembly block starts at a open brace `{` and ends at a matching close brace `}`.
 * The text between braces must have balanced brackets, brackets inside strings are ignored.
 * 
 *  @param p The file_cursor to read from.
 *  @param open_bracket The open bracket to search for the equivelant close bracket.
 */
[[nodiscard]] std::optional<token> parse_bracketed_string(char const*& p, char open_bracket);

[[nodiscard]] std::optional<token> parse_block_comment(char const*& p);
[[nodiscard]] std::optional<token> parse_identifier(char const*& p);
[[nodiscard]] std::optional<token> parse_tag(char const*& p);
[[nodiscard]] std::optional<token> parse_context_arg(char const*& p);
[[nodiscard]] std::optional<token> parse_position_arg(char const*& p);
[[nodiscard]] std::optional<token> parse_line_comment(char const*& p);
[[nodiscard]] std::optional<token> parse_line_directive(char const*& p);
[[nodiscard]] std::optional<token> parse_number(char const*& p);
[[nodiscard]] std::optional<token> parse_superscript_integer(char const*& p);
[[nodiscard]] std::optional<token> parse_operator(char const*& p);
[[nodiscard]] std::optional<token> parse_scram_directive(char const*& p);
[[nodiscard]] std::optional<token> parse_string(char const*& p);


}
