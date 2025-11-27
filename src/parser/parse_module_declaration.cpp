
#include "parse_module_declaration.hpp"
#include "parse_fqname.hpp"
#include "tokenizer/token_parsers.hpp"
#include "error/errors.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::module_declaration_node> parse_module_declaration(token_iterator& it, parse_context &ctx)
{
    auto const first = it[0].begin();

    if (*it != "module") {
        return tokens_did_not_match;
    }
    ++it;

    auto r = std::make_unique<ast::module_declaration_node>(first);

    if (auto node = parse_fqname(it, ctx, false)) {
        r->name = std::move(node).value();
    } else if (node.error()) {
        return std::unexpected{node.error()};
    } else {
        return ctx.add_error(first, it->end(), error::missing_fqname);
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
