
#include "parse_import_repository_declaration.hpp"
#include "parse_build_guard.hpp"
#include "ast/build_guard_literal_node.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::import_repository_declaration_node> parse_import_repository_declaration(token_iterator& it, parse_context &ctx)
{
    auto const first = it->begin();

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
        url = it->raw_string_value();
        ++it;
    } else {
        return ctx.add(first, it->end(), hkc_error::missing_git_url);
    }

    auto rev = std::string{};
    if (type == repository_type::git) {
        if (*it == token::string_literal) {
            rev = it->raw_string_value();
            ++it;
        } else {
            return ctx.add(first, it->end(), hkc_error::missing_git_rev);
        }
    }

    auto r = std::make_unique<ast::import_repository_declaration_node>(first);
    r->url = repository_url{type, std::move(url), std::move(rev)};

    if (auto optional_build_guard = parse_build_guard(it, ctx)) {
        r->build_guard = std::move(optional_build_guard).value();
    } else if (to_bool(optional_build_guard.error())) {
        return std::unexpected{optional_build_guard.error()};
    } else {
        r->build_guard = std::make_unique<ast::build_guard_literal_node>(it->begin(), it->begin(), true);
    }

    if (*it != ';') {
        return ctx.add(first, it->end(), hkc_error::missing_semicolon);
    }
    ++it;

    return r;
}

} // namespace hk