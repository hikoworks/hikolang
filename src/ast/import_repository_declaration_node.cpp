
#include "import_repository_declaration_node.hpp"

namespace hk::ast {

[[nodiscard]] generator<std::pair<repository_url, error_location>>
import_repository_declaration_node::remote_repositories(error_list& errors) const
{
    co_yield {url, error_location{errors, first, last}};
}

} // namespace hk::ast
