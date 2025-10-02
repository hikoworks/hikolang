
#pragma once

#include "parse_result.hpp"
#include "ast/nodes.hpp"
#include "tokenizer/token.hpp"
#include "tokenizer/token_vector.hpp"
#include "error/error_list.hpp"
#include "utility/file_cursor.hpp"
#include "utility/generator.hpp"
#include <vector>
#include <memory>
#include <expected>

namespace hk {




[[nodiscard]] parse_result<fqname> parse_fqname(token_iterator& it, error_list& e);
[[nodiscard]] parse_result_ptr<ast::node> parse(hk::file_cursor& c);
[[nodiscard]] parse_result_ptr<ast::node> parse(token_iterator& it, error_list &errors);
[[nodiscard]] parse_result_ptr<ast::module_declaration_node> parse_module_declaration(token_iterator& it, error_list& e);
[[nodiscard]] parse_result_ptr<ast::import_declaration_node> parse_import_declaration(token_iterator& it, error_list& e);

/** Parse a module.
 * 
 * @param it A token iterator.
 * @param errors[out] error list output.
 * @param only_prologue When true only parses the prologue of a module.
 * @return A module node if successful. 
 */
[[nodiscard]] parse_result_ptr<ast::module_node> parse_module(token_iterator& it, error_list &errors, bool only_prologue);

}