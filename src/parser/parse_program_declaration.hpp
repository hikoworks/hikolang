
#pragma once

#include "parse_result.hpp"
#include "ast/program_declaration_node.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::program_declaration_node> parse_program_declaration(token_iterator& it, error_list& e);

}
