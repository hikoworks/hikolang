
#pragma once

#include "node.hpp"

namespace hk::ast {

class attribute_node : public node {
public:
    constexpr attribute_node(char const* first, char const* last = nullptr) noexcept :
        node(first, last)
    {
    }

private:
    std::string _name;
    std::unique_ptr<node> _content;
};

}
