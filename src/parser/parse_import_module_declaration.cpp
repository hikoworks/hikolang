
#include "parse_import_module_declaration.hpp"
#include "parse_fqname.hpp"
#include "error/errors.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::import_module_declaration_node> parse_import_module_declaration(token_iterator& it, parse_context &ctx)
{
    auto const first = it->first;

    if (it[0] != "import") {
        return tokens_did_not_match;
    }
    ++it;

    auto r = std::make_unique<ast::import_module_declaration_node>(first);

    if (auto name = parse_fqname(it, ctx, false)) {
        r->name = std::move(name).value();
        ++it;
    } else if (name.error()) {
        return std::unexpected{name.error()};
    } else {
        return ctx.e.add(first, it->last, error::missing_module_name);
    }

    if (*it == "as") {
        ++it;
        if (auto as = parse_fqname(it, ctx, true)) {
            r->as = std::move(as).value();
            ++it;
        } else if (as.error()) {
            return std::unexpected{as.error()};
        } else {
            return ctx.e.add(first, it->last, error::missing_as_name);
        }
    }

    if (*it != ';') {
        return ctx.e.add(first, it->last, error::missing_semicolon);
    }

    ++it;
    return r;
}

} // namespace hk