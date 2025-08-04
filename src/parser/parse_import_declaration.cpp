
#include "parsers.hpp"
#include "error/errors.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::import_declaration_node> parse_import_declaration(token_iterator& it, error_list& e)
{
    auto const first = it->first;

    if (*it != "import") {
        return tokens_did_not_match;
    }
    ++it;

    auto r = std::make_unique<ast::import_declaration_node>(first);

    if (*it == "git") {
        r->kind = ast::import_declaration_node::kind_type::git;
        ++it;

        if (*it == token::string_literal) {
            r->path = it->text;
            ++it;
        } else {
            return e.add<error::missing_import_git_declaration_url>(first, it->last);
        }

        if (*it == token::string_literal) {
            r->branch = it->text;
            ++it;
        } else {
            return e.add<error::missing_import_git_declaration_branch>(first, it->last);
        }

    } else if (*it == "zip") {
        r->kind = ast::import_declaration_node::kind_type::zip;
        ++it;

        if (*it == token::string_literal) {
            r->path = it->text;
            ++it;
        } else {
            return e.add<error::missing_import_zip_declaration_path>(first, it->last);
        }

    } else if (*it == "lib") {
        r->kind = ast::import_declaration_node::kind_type::lib;
        ++it;

        if (*it == token::string_literal) {
            r->path = it->text;
            ++it;
        } else {
            return e.add<error::missing_import_lib_declaration_path>(first, it->last);
        }

    } else if (auto fqname = parse_fqname(it, e)) {
        r->kind = ast::import_declaration_node::kind_type::mod;
        r->name = std::move(fqname).value();
        ++it;

        if (*it == "as") {
            ++it;

            if (*it == token::string_literal) {
                r->as = it->text;

            } else {
                return e.add<error::missing_import_mod_declaration_as_name>(first, it->last);
            }
        }

    } else if (fqname.error()) {
        return std::unexpected{fqname.error()};
    } else {
        return e.add<error::missing_import_mod_declaration_name>(first, it->last);
    }

    if (*it == ';') {
        ++it;
        return r;
    }

    return e.add<error::missing_import_declaration_semicolon>(first, it->last);
}

}