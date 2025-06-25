
#include "parsers.hpp"

namespace hl {

[[nodiscard]] std::unique_ptr<ast::module_declaration_node> parse_module_declaration(token_iterator& it, token_iterator it_end)
{
    if (it[0] == "module") {
        ++it;
    } else {
        return nullptr;
    }

    auto r = std::make_unique<ast::module_declaration_node>();

    if (auto node = parse_fqname(it, it_end)) {
        r->name = std::move(node);
    } else {
        return errors.add(it, "Expected fully qualified name after 'module' keyword");
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