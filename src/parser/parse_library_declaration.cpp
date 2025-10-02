
#include "parsers.hpp"
#include "tokenizer/token_parsers.hpp"
#include "error/errors.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::module_declaration_node> parse_module_declaration(token_iterator& it, error_list& e)
{
    auto const first = it[0].first;

    auto kind = ast::module_declaration_node::kind_type{};
    if (*it == "module") {
        kind = ast::module_declaration_node::kind_type::module;
    } else if (*it == "program") {
        kind = ast::module_declaration_node::kind_type::program;
    } else if (*it == "library") {
        kind = ast::module_declaration_node::kind_type::library;
    } else {
        return tokens_did_not_match;
    }
    ++it;

    auto r = std::make_unique<ast::module_declaration_node>(first);

    if (kind == ast::module_declaration_node::kind_type::module) {
        if (auto node = parse_fqname(it, e)) {
            r->name = std::move(node).value();
        } else if (node.error()) {
            return std::unexpected{node.error()};
        } else {
            return e.add(first, it->last, error::missing_module_declaration_name);
        }

    } else if (*it == token::string_literal) {
        r->filename_stem = it->text();
        ++it;

    } else {
        return e.add(first, it->last, error::missing_filename_stem);
    }

    if (*it == token::version_literal) {
        r->version = it->version_literal();
        ++it;
    }

    // Optional conditional compilation.
    if (*it == "fallback") {
        r->is_fallback = true;
        ++it;

    } else if (*it == "if") {
        ++it;
        return e.add(first, it->last, error::unimplemented_module_declaration_if);
    }

    if (*it == ';') {
        ++it;
        return r;
    }

    return e.add(first, it->last, error::missing_semicolon);
}

} // namespace hk
