
#pragma once

#include "parse_context.hpp"
#include "parse_context.hpp"
#include "ast/module_node.hpp"
#include "tokenizer/file_cursor.hpp"
#include "tokenizer/token_vector.hpp"

namespace hk {

/** Parse a module from the given file cursor.
 * 
 * Even when there are errors this function will return a module-node.
 * 
 * @note The file should be a module file with the `.hkm` extension.
 * @param c The file cursor to parse from.
 * @param only_prologue If true, only parse the prologue of the module (declaration and imports).
 * @return A parsed module, or what was parsed.
 */
[[nodiscard]] ast::module_node_ptr parse_module(token_iterator& it, parse_context &ctx, bool only_prologue);


}