
#include "module_node.hpp"
#include "import_repository_declaration_node.hpp"

namespace hk::ast {

[[nodiscard]] generator<repository_url> module_node::remote_repositories() const
{
    for (auto const& import : imports) {
        if (auto repository_import_ptr = dynamic_cast<import_repository_declaration_node const*>(import.get())) {
            co_yield repository_import_ptr->url;
        }
    }
}

}