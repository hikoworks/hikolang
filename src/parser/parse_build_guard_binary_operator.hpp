
#pragma once

#include "parse_result.hpp"
#include "parse_context.hpp"
#include "ast/build_guard_binary_operator_node.hpp"
#include "tokenizer/token_vector.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::build_guard_binary_operator_node>
parse_build_guard_binary_operator(token_iterator& it, parse_context& ctx);

}
