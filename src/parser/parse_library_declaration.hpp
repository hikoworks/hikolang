
#pragma once

#include "parse_result.hpp"
#include "file_parse_context.hpp"
#include "ast/library_declaration_node.hpp"
#include "error/error_list.hpp"
#include "tokenizer/token_vector.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::library_declaration_node> parse_library_declaration(token_iterator& it, file_parse_context &ctx);

}
