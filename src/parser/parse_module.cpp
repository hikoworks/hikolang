
#include "parsers.hpp"
#include "consume.hpp"

namespace hl {


[[nodiscard]] parse_result<ast::module_node> parse_module(token_iterator& it, error_list& errors)
{
    auto module_node = std::make_unique<ast::module_node>();

    if (auto node = parse_module_declaration(it, errors)) {
        module_node->declaration = std::move(node).value();
    } else if (node.has_error()) {
        return node.error();
    } else {
        return nullptr;
    }

    while (true) {
        if (auto node = parse_import_declaration(it, errors)) {
            module_node->imports.push_back(std::move(node).value());
        } else if (node.has_error()) {
            // Parsing can continue on failure.
            consume_rest_of_statement(it);
        } else {
            break;
        }
    }

    return module_node;
}

} // namespace hl