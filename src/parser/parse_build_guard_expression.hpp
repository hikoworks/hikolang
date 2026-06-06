
#pragma once

#include "parse_result.hpp"
#include "file_parse_context.hpp"
#include "ast/build_guard_expression_node.hpp"
#include "tokenizer/token_vector.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::build_guard_expression_node> parse_build_guard_expression(
    token_iterator& it,
    file_parse_context& ctx,
    std::unique_ptr<ast::build_guard_expression_node> lhs,
    size_t min_precedence);

}