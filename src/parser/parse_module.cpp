
#include "parsers.hpp"

namespace hl {

[[nodiscard]] std::unique_ptr<ast::module_node> parse_module(token_iterator& it, token_iterator it_end)
{
    auto module_node = std::make_unique<ast::module_node>();

    if (auto node = parse_module_declaration(it, it_end)) {
        module_node->declaration = std::move(node);
    } else {
        return nullptr;
    }

    while (auto node = parse_import_declaration(it, it_end)) {
        module_node->imports.push_back(std::move(node));
    }



    return module_node;
}

} // namespace hl