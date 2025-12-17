
#include "parse_library_declaration.hpp"
#include "parse_build_guard.hpp"
#include "tokenizer/token_parsers.hpp"

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
        return ctx.add(first, it->end(), hkc_error::missing_filename_stem);
    }

    if (*it == token::version_literal) {
        r->version = it->version_value();
        ++it;
    }

    if (auto optional_build_guard = parse_build_guard(it, ctx)) {
        r->build_guard = std::move(optional_build_guard).value();
    } else if (to_bool(optional_build_guard.error())) {
        return std::unexpected{optional_build_guard.error()};
    }

    if (*it != ';') {
        return ctx.add(first, it->end(), hkc_error::missing_semicolon);
    }

    ++it;
    return r;
}

} // namespace hk
