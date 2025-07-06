
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

/** Parse a module from the given file cursor.
 * 
 * @note The file should be a module file with the `.hm` extension.
 * @param c The file cursor to parse from.
 * @param [out] e The error list to add errors to.
 * @param only_prologue If true, only parse the prologue of the module (declaration and imports).
 * @return A parse result containing the module node or an error code.
 */
[[nodiscard]] parse_result_ptr<ast::module_node> parse_module(hk::file_cursor& c, error_list &e, bool only_prologue);

/** Parse a package metadata from the given file cursor. 
 * 
 * @note The file should be a package file with the `.hp` extension.
 * @param c The file cursor to parse from.
 * @param [out] e The error list to add errors to.
 * @param only_prologue If true, only parse the prologue of the package (declaration and imports).
 * @return A parse result containing the package node or an error code.
 */
[[nodiscard]] parse_result_ptr<ast::package_node> parse_package(hk::file_cursor& c, error_list &e, bool only_prologue);

[[nodiscard]] parse_result<fqname> parse_fqname(token_iterator& it, error_list& e);
[[nodiscard]] parse_result_ptr<ast::node> parse(hk::file_cursor& c);
[[nodiscard]] parse_result_ptr<ast::node> parse(token_iterator& it, error_list &errors);
[[nodiscard]] parse_result_ptr<ast::module_node> parse_module(token_iterator& it, error_list &errors, bool only_prologue);
[[nodiscard]] parse_result_ptr<ast::module_declaration_node> parse_module_declaration(token_iterator& it, error_list& e);
[[nodiscard]] parse_result_ptr<ast::import_declaration_node> parse_import_declaration(token_iterator& it, error_list& e);

}