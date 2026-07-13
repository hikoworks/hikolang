
#pragma once

#include "node.hpp"

namespace hk::ast {

class documentation_node : public node {
public:
    constexpr documentation_node(char const* first, char const* last = nullptr) noexcept :
        node(first, last)
    {
    }

private:
    std::string _text;
};

}
