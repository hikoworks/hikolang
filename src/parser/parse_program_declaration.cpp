
#include "parse_program_declaration.hpp"
#include "tokenizer/token_parsers.hpp"
#include "error/errors.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::program_declaration_node> parse_program_declaration(token_iterator& it, parse_context &ctx)
{
    auto const first = it[0].first;

    if (*it != "program") {
        return tokens_did_not_match;
    }
    ++it;

    auto r = std::make_unique<ast::program_declaration_node>(first);

    if (*it == token::string_literal) {
        r->filename_stem = it->text;
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

    if (*it == ';') {
        ++it;
        return r;
    }

    return ctx.e.add(first, it->last, error::missing_semicolon);
}

} // namespace hk
