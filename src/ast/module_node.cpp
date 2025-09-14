
#include "module_node.hpp"
#include "import_repository_declaration_node.hpp"

namespace hk::ast {

[[nodiscard]] generator<std::pair<repository_url, error_location>> module_node::remote_repositories() const
{
    for (auto const& import : imports) {
        for (auto result : import->remote_repositories(errors)) {
            co_yield std::move(result);
        }
    }
}

}