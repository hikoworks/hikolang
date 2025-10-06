
#include "parse_import_repository_declaration.hpp"
#include "error/errors.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::import_repository_declaration_node> parse_import_repository_declaration(token_iterator& it, parse_context &ctx)
{
    auto const first = it->first;

    if (it[0] != "import" and (it[1] == "git" or it[1] == "zip") and it[2] == token::string_literal) {
        return tokens_did_not_match;
    }

    auto const type = [&]{
        if (it[1] == "git") {
            return repository_type::git;
        } else if (it[1] == "zip") {
            return repository_type::zip;
        } else {
            std::unreachable();
        }
    }();
    it += 2;

    auto url = std::string{};
    if (*it == token::string_literal) {
        url = it->text;
        ++it;
    } else {
        return ctx.e.add(first, it->last, error::missing_git_url);
    }

    auto rev = std::string{};
    if (type == repository_type::git) {
        if (*it == token::string_literal) {
            rev = it->text;
            ++it;
        } else {
            return ctx.e.add(first, it->last, error::missing_git_rev);
        }
    }

    if (*it != ';') {
        return ctx.e.add(first, it->last, error::missing_semicolon);
    }
    ++it;

    auto r = std::make_unique<ast::import_repository_declaration_node>(first);
    r->url = repository_url{type, std::move(url), std::move(rev)};
    return r;
}

} // namespace hk