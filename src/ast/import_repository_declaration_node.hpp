
#include "import_declaration_node.hpp"
#include "utility/repository_url.hpp"

namespace hk::ast {

class import_repository_declaration_node : public import_declaration_node {
public:
    using import_declaration_node::import_declaration_node;

    import_repository_declaration_node(file_location first, file_location last, repository_url url) :
        import_declaration_node(first, last), url(url)
    {
    }

    repository_url url;
};

} // namespace hk::ast
