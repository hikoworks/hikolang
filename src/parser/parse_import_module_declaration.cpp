
#include "parse_import_module_declaration.hpp"
#include "parse_fqname.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::import_module_declaration_node> parse_import_module_declaration(token_iterator& it, parse_context &ctx)
{
    auto const first = it->begin();

    if (it[0] != "import") {
        return tokens_did_not_match;
    }
    ++it;

    auto r = std::make_unique<ast::import_module_declaration_node>(first);

    if (auto name = parse_fqname(it, ctx, false)) {
        r->name = std::move(name).value();
        ++it;
    } else if (not name.error()) {
        return ctx.add(first, it->end(), hkc_error::missing_module_name);
    } else {
        return std::unexpected{name.error()};
    }

    if (*it == "as") {
        ++it;
        if (auto as = parse_fqname(it, ctx, true)) {
            r->as = std::move(as).value();
            ++it;
        } else if (to_bool(as.error())) {
            return std::unexpected{as.error()};
        } else {
            return ctx.add(first, it->end(), hkc_error::missing_as_name);
        }
    }

    if (*it != ';') {
        return ctx.add(first, it->end(), hkc_error::missing_semicolon);
    }

    ++it;
    return r;
}

} // namespace hk