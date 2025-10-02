
#include "node.hpp"
#include "utility/repository_url.hpp"

namespace hk::ast {

class import_repository_declaration_node : public node {
public:
    repository_url url;

    using node::node;

    import_repository_declaration_node(file_location first, file_location last, repository_url url) :
        node(first, last), url(url)
    {
    }

};

using module_node_ptr = std::unique_ptr<import_repository_declaration_node>;

} // namespace hk::ast
