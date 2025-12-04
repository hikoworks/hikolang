
#pragma once

#include "parse_result.hpp"
#include "parse_context.hpp"
#include "tokenizer/token_vector.hpp"
#include "tokenizer/token.hpp"
#include "ast/top_node.hpp"
#include <vector>
#include <memory>
#include <expected>

namespace hk {

/** Parse a module.
 * 
 * @param[in] it A token iterator.
 * @param[in,out] ctx error list output.
 * @param only_prologue When true only parses the prologue of a module.
 * @return A module node if successful. 
 */
[[nodiscard]] parse_result_ptr<ast::top_node> parse_top(token_iterator& it, parse_context &ctx, bool only_prologue);

/** Parse a module.
 *
 * @param[in] p A pointer to the text to parse, text must have at least 8 nul
 *              character at the end.
 * @param[in,out] ctx error list output.
 * @param only_prologue When true only parses the prologue of a module.
 * @return A module node if successful. 
 */
[[nodiscard]] parse_result_ptr<ast::top_node> parse_top(char const* p, parse_context &ctx, bool only_prologue);

}