
#pragma once

#include "error/hkc_error.hpp"
#include <string>
#include <memory>
#include <variant>
#include <cassert>
#include <expected>
#include <format>

namespace hk::ast {

class repository;

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

    constexpr node(char const* first, char const* last = nullptr) noexcept :
        first(first), last(last)
    {
    }

    [[nodiscard]] node& parent() const noexcept
    {
        assert(_parent != nullptr);
        return *_parent;
    }

    node& set_parent(node &parent) noexcept
    {
        _parent = &parent;
        return *this;
    }

    [[nodiscard]] node& top() const noexcept
    {
        return _parent ? _parent->top() : *const_cast<node *>(this);
    }

    [[nodiscard]] virtual hk::ast::repository& repository() const
    {
        return top().repository();
    }

    std::unexpected<hkc_error> add(hkc_error error)
    {
        using namespace std::literals;
        return _add(error, ""s);
    }

    template<typename... Args>
    std::unexpected<hkc_error> add(hkc_error error, std::format_string<Args...> fmt, Args&&...args)
    {
        return _add(error, std::format(std::move(fmt), std::forward<Args>(args)...));
    }


protected:
    node *_parent = nullptr;

private:
    std::unexpected<hkc_error> _add(hkc_error error, std::string message);

};

using node_ptr = std::unique_ptr<node>;

} // namespace hk::ast
