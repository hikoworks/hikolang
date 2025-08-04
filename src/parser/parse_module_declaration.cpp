
#include "parsers.hpp"
#include "tokenizer/token_parsers.hpp"
#include "error/errors.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::module_declaration_node> parse_module_declaration(token_iterator& it, error_list& e)
{
    auto const first = it[0].first;

    if (*it != "module") {
        return tokens_did_not_match;
    }
    ++it;

    auto r = std::make_unique<ast::module_declaration_node>(first);

    if (auto node = parse_fqname(it, e)) {
        r->name = std::move(node).value();
    } else if (node.error()) {
        return std::unexpected{node.error()};
    } else {
        return e.add<error::missing_module_declaration_name>(first, it->last);
    }

    // Optional module specialization.
    if (*it == "application") {
        ++it;
        r->kind = ast::module_declaration_node::kind_type::application;
        if (*it == token::string_literal) {
            r->output_filename = it->text;
            ++it;
        } else {
            return e.add<error::missing_module_application_declaration_exe>(first, it->last);
        }

    } else if (*it == "library") {
        ++it;
        r->kind = ast::module_declaration_node::kind_type::library;
        if (*it == token::string_literal) {
            r->output_filename = it->text;
            ++it;
        } else {
            return e.add<error::missing_module_library_declaration_bin>(first, it->last);
        }

    } else if (*it == "package") {
        ++it;
        r->kind = ast::module_declaration_node::kind_type::package;
        if (*it == token::version_literal) {
            r->version = it->version_value();
            ++it;
        } else {
            return e.add<error::missing_module_package_declaration_version>(first, it->last);
        }

    } else {
        r->kind = ast::module_declaration_node::kind_type::module;
    }

    // Optional conditional compilation.
    if (*it == "fallback") {
        ++it;
        r->is_fallback = true;

    } else if (*it == "if") {
        ++it;
        return e.add<error::unimplemented_module_declaration_if>(first, it->last);
    }

    if (*it == ';') {
        ++it;
        return r;
    }

    return e.add<error::missing_module_declaration_semicolon>(first, it->last);
}

} // namespace hk
