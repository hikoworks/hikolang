
#pragma once

#include <string>
#include <memory>
#include <variant>

namespace hk::ast {

class repository;

class node {
public:
    void *_parent;

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

    constexpr node(repository& parent, char const* first, char const* last = nullptr) noexcept :
        _parent(&parent), first(first), last(last)
    {
    }

    constexpr node(node& parent, char const* first, char const* last = nullptr) noexcept :
        _parent(&parent), first(first), last(last)
    {
    }

    [[nodiscard]] virtual hk::ast::repository& repository() const
    {
        return static_cast<node *>(_parent)->repository();
    }
};

using node_ptr = std::unique_ptr<node>;

} // namespace hk::ast
