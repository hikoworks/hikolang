
#pragma once

#include "ast/nodes.hpp"
#include "tokenizer/token.hpp"
#include "error/error_list.hpp"
#include "utility/file_cursor.hpp"
#include <vector>
#include <memory>
#include <expected>

namespace hk {

using token_iterator = std::vector<token>::const_iterator;

template<typename T>
using parse_result = std::expected<T, error_code>;

template<std::derived_from<ast::node> T>
using parse_result_ptr = parse_result<std::unique_ptr<T>>;

constexpr std::unexpected<error_code> tokens_did_not_match = std::unexpected{error_code{}};


[[nodiscard]] parse_result<fqname> parse_fqname(token_iterator& it, error_list& e);
[[nodiscard]] parse_result_ptr<ast::node> parse(hk::file_cursor& c);
[[nodiscard]] parse_result_ptr<ast::node> parse(token_iterator& it, error_list &errors);
[[nodiscard]] parse_result_ptr<ast::module_node> parse_module(token_iterator& it, error_list &errors, bool only_prologue);
[[nodiscard]] parse_result_ptr<ast::module_declaration_node> parse_module_declaration(token_iterator& it, error_list& e);
[[nodiscard]] parse_result_ptr<ast::import_declaration_node> parse_import_declaration(token_iterator& it, error_list& e);

}