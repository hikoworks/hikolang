
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

    auto r = std::unique_ptr<ast::import_declaration_node>{};

    if (*it == "git") {
        ++it;

        auto url = std::string{};
        if (*it == token::string_literal) {
            url = it->text;
            ++it;
        } else {
            return e.add(first, it->last, error::missing_import_git_declaration_url);
        }

        auto rev = std::string{};
        if (*it == token::string_literal) {
            rev = it->text;
            ++it;
        } else {
            return e.add(first, it->last, error::missing_import_git_declaration_branch);
        }

        auto repository = repository_url{repository_type::git, std::move(url), std::move(rev)};
        r = std::make_unique<ast::import_repository_declaration_node>(first, it->last, std::move(repository));

    } else if (*it == "zip") {
        ++it;

        auto url = std::string{};
        if (*it == token::string_literal) {
            url = it->text;
            ++it;
        } else {
            return e.add(first, it->last, error::missing_import_zip_declaration_path);
        }

        auto repository = repository_url{repository_type::zip, it->text};
        r = std::make_unique<ast::import_repository_declaration_node>(first, it->last, std::move(repository));

    } else if (*it == "lib") {
        ++it;

        auto path = std::string{};
        if (*it == token::string_literal) {
            path = it->text;
            ++it;
        } else {
            return e.add(first, it->last, error::missing_import_lib_declaration_path);
        }

        r = std::make_unique<ast::import_library_declaration_node>(first, it->last, path);

    } else if (auto name = parse_fqname(it, e)) {
        ++it;

        auto as = hk::fqname{};
        if (*it == "as") {
            ++it;

            if (auto as = parse_fqname(it, e)) {
                as = std::move(as).value();

            } else {
                return e.add(first, it->last, error::missing_import_mod_declaration_as_name);
            }
        }

        r = std::make_unique<ast::import_module_declaration_node>(first, it->last, std::move(name).value(), as);

    } else if (name.error()) {
        return std::unexpected{name.error()};

    } else {
        return e.add(first, it->last, error::missing_import_mod_declaration_name);
    }

    if (*it == ';') {
        ++it;
        return r;
    }

    return e.add(first, it->last, error::missing_import_declaration_semicolon);
}

} // namespace hk