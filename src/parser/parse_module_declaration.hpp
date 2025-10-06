
#pragma once

#include "parse_result.hpp"
#include "parse_context.hpp"
#include "ast/module_declaration_node.hpp"
#include "error/error_list.hpp"
#include "tokenizer/token_vector.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::module_declaration_node> parse_module_declaration(token_iterator& it, parse_context &ctx);

}
