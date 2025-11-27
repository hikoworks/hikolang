
#include "parse_library_declaration.hpp"
#include "tokenizer/token_parsers.hpp"
#include "error/errors.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::library_declaration_node> parse_library_declaration(token_iterator& it, parse_context &ctx)
{
    auto const first = it[0].begin();

    if (*it != "library") {
        return tokens_did_not_match;
    }
    ++it;

    auto r = std::make_unique<ast::library_declaration_node>(first);

    if (*it == token::string_literal) {
        r->filename_stem = it->raw_string_value();
        ++it;

    } else {
        return ctx.add_error(first, it->end(), error::missing_filename_stem);
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
        return ctx.add_error(first, it->end(), error::unimplemented);
    }

    if (*it != ';') {
        return ctx.add_error(first, it->end(), error::missing_semicolon);
    }

    ++it;
    return r;
}

} // namespace hk
