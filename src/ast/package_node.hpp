
#include "node.hpp"
#include "condition_expression_node.hpp"
#include "package_import_node.hpp"
#include "utility/fqname.hpp"
#include "utility/semantic_version.hpp"

namespace hl::ast {

class package_node : public node {
public:
    using node::node;

    fqname name;
    semantic_version version;
    std::unique_ptr<condition_expression_node> condition;
    std::vector<std::unique_ptr<package_import_node>> imports;

};

}
