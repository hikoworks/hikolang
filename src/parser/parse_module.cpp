
#include "parsers.hpp"
#include "consume.hpp"
#include "ast/nodes.hpp"


namespace hl {


[[nodiscard]] parse_result_ptr<ast::module_node> parse_module(token_iterator& it, error_list& errors)
{
    auto const first = it->first;
    auto r = std::make_unique<ast::module_node>(first);

    if (auto node = parse_module_declaration(it, errors)) {
        r->declaration = std::move(node).value();
    } else if (node.error()) {
        return std::unexpected{node.error()};
    } else {
        return tokens_did_not_match;
    }

    while (true) {
        if (auto node = parse_import_declaration(it, errors)) {
            r->imports.push_back(std::move(node).value());
        } else if (node.error()) {
            // Parsing can continue on failure.
            consume_rest_of_statement(it);
        } else {
            // Something else than a `import` declaration was found.
            break;
        }
    }

    auto const last = it->first;
    r->last = last;
    return r;
}

} // namespace hl