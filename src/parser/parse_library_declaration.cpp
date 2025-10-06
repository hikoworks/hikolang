
#include "parse_library_declaration.hpp"
#include "tokenizer/token_parsers.hpp"
#include "error/errors.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::library_declaration_node> parse_library_declaration(token_iterator& it, parse_context &ctx)
{
    auto const first = it[0].first;

    if (*it != "library") {
        return tokens_did_not_match;
    }
    ++it;

    auto r = std::make_unique<ast::library_declaration_node>(first);

    if (*it == token::string_literal) {
        r->filename_stem = std::move(it->text);
        ++it;

    } else {
        return ctx.e.add(first, it->last, error::missing_filename_stem);
    }

    if (*it == token::version_literal) {
        r->version = it->version_value();
        ++it;
    }

    // Optional conditional compilation.
    if (*it == "fallback") {
        r->is_fallback = true;
        ++it;

    } else if (*it == "if") {
        ++it;
        return ctx.e.add(first, it->last, error::unimplemented);
    }

    if (*it != ';') {
        return ctx.e.add(first, it->last, error::missing_semicolon);
    }

    ++it;
    return r;
}

} // namespace hk
