
#pragma once

#include "parse_result.hpp"
#include "parse_context.hpp"
#include "ast/import_repository_declaration_node.hpp"
#include "tokenizer/token_vector.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::import_repository_declaration_node> parse_import_repository_declaration(token_iterator& it, parse_context &ctx);

}
