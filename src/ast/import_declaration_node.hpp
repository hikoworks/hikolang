
#pragma once

#include "node.hpp"
#include "utility/fqname.hpp"
#include "utility/generator.hpp"
#include "utility/repository_url.hpp"
#include "error/error_location.hpp"

namespace hk::ast {

class import_declaration_node : public node {
public:
    using node::node;

    [[nodiscard]] virtual generator<std::pair<repository_url, error_location>> remote_repositories(error_list &errors) const
    {
        co_return;
    }
};

using import_declaration_node_ptr = std::unique_ptr<import_declaration_node>;

}
