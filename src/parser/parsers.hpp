
#pragma once

#include "ast/node.hpp"
#include "tokenizer/token.hpp"
#include <vector>
#include <memory>

namespace hl {

using token_iterator = std::vector<token>::const_iterator;

[[nodiscard]] std::unique_ptr<ast::node> parse_module(token_iterator& it, token_iterator it_end);

}