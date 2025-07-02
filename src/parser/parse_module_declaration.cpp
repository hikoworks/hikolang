
#include "parsers.hpp"

namespace hl {

[[nodiscard]] std::unique_ptr<ast::module_declaration_node> parse_module_declaration(token_iterator& it, error_list& e)
{
    auto const first = it[0].first;

    if (it[0] == "module") {
        ++it;
    } else {
        return nullptr;
    }

    auto r = std::make_unique<ast::module_declaration_node>();

    if (auto node = parse_fqname(it, it_end)) {
        r->name = std::move(node);
    } else {
        return e.add<"E0001: Expected fully qualified name after 'module' keyword">(first, it->last);
    }

    if (it[0] == "fallback") {
        r->is_fallback = true;
        ++it;

    } else if (it[0] == "if") {
        ++it;
        if (auto condition = parse_condition_expression(it, it_end)) {
            r->condition = std::move(condition);
        } else {
            return errors.add(it, "Expected expression after 'if' in module declaration");
        }
    }

    if (it[0] != ';') {
        return error.add(it, "Expected ';' after module declaration");
    }

    return r;
}

} // namespace hl