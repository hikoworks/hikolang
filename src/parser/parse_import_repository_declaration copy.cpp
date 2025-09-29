
#include "parsers.hpp"
#include "error/errors.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::import_repository_declaration_node> parse_import_repository_declaration(token_iterator& it, error_list& e)
{
    auto const first = it->first;

    if (it[0] != "import") {
        return tokens_did_not_match;
    }

    auto needs_rev = false;
    if (it[1] == "git") {
        needs_rev = true;
    } else if (it[1] == "zip") {
        needs_rev = false;
    } else {
        return tokens_did_not_match;
    }
    it += 2;

    auto url = std::string{};
    if (*it == token::string_literal) {
        url = it->text;
        ++it;
    } else {
        return e.add(first, it->last, error::missing_import_git_declaration_url);
    }

    auto rev = std::string{};
    if (needs_rev) {
        if (*it == token::string_literal) {
            rev = it->text;
            ++it;
        } else {
            return e.add(first, it->last, error::missing_import_git_declaration_branch);
        }
    }

    if (*it != ';') {
        return e.add(first, it->last, error::missing_import_declaration_semicolon);
    }
    ++it;

    return repository_url{repository_type::git, std::move(url), std::move(rev)};
}

} // namespace hk