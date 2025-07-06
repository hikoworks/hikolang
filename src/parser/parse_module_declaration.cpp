
#include "parsers.hpp"

namespace hk {

[[nodiscard]] parse_result_ptr<ast::module_declaration_node> parse_module_declaration(token_iterator& it, error_list& e)
{
    auto const first = it[0].first;

    if (*it != "module") {
        return tokens_did_not_match;
    }
    ++it;

    auto r = std::make_unique<ast::module_declaration_node>(first);

    if (auto node = parse_fqname(it, e)) {
        r->name = std::move(node).value();
    } else if (node.error()) {
        return std::unexpected{node.error()};
    } else {
        return e.add<"E0001: Expected fully qualified name after 'module' keyword in module-declaration.">(first, it->last);
    }

    if (*it == "fallback") {
        r->is_fallback = true;
        ++it;

    //} else if (*it == "if") {
    //    ++it;
//
    //    if (auto condition = parse_condition_expression(it, e)) {
    //        r->condition = std::move(condition);
    //    } else if (condition.has_error()) {
    //        return condition.error();
    //    } else {
    //        return e.add<"E0002: Expected expression after 'if' in module-declaration.">(first, it->last);
    //    }
    }

    auto const last = it->first;
    if (*it != ';') {
        return e.add<"E0003: Expected ';' after module-declaration.">(first, it->last);
    }
    ++it;

    r->last = last;
    return r;
}

} // namespace hk