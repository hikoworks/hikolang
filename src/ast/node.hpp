
#pragma once

#include <string>
#include <memory>

namespace hk::ast {

class node {
public:
    /** The location in the file where the first character of a node is located.
     */
    char const* first = nullptr;

    /** One position after the location of the last character of the node.
     */
    char const* last = nullptr;

    /** Description of the node.
     */
    std::string description = {};

    virtual ~node() = default;
    constexpr node(char const* first) noexcept : first(first), last(nullptr) {}
};

using node_ptr = std::unique_ptr<node>;

} // namespace hk::ast
