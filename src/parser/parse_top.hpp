
#pragma once

#include "parse_result.hpp"
#include "parse_context.hpp"
#include "tokenizer/token_vector.hpp"
#include "tokenizer/token.hpp"
#include "tokenizer/file_cursor.hpp"
#include "ast/top_node.hpp"
#include <vector>
#include <memory>
#include <expected>

namespace hk {

/** Parse a file.
 * 
 * @param it A token iterator.
 * @param errors[out] error list output.
 * @param only_prologue When true only parses the prologue of a module.
 * @return A module node if successful. 
 */
[[nodiscard]] parse_result_ptr<ast::top_node> parse_top(token_iterator& it, parse_context &ctx, bool only_prologue);

[[nodiscard]] parse_result_ptr<ast::top_node> parse_top(hk::file_cursor& c, parse_context &ctx, bool only_prologue);

}