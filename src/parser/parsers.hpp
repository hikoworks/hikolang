
#pragma once

#include "parse_result.hpp"
#include "ast/node.hpp"
#include "tokenizer/token.hpp"
#include "error/error_list.hpp"
#include <vector>
#include <memory>

namespace hl {

using token_iterator = std::vector<token>::const_iterator;

[[nodiscard]] parse_result<ast::node> parse(token_iterator& it, error_list &errors)

[[nodiscard]] parse_result<ast::node> parse_module(token_iterator& it, error_list &errors);

}