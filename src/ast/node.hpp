
#pragma once

#include "error/hkc_error.hpp"
#include "tokenizer/line_table.hpp"
#include "utility/generator.hpp"
#include "utility/datum_namespace.hpp"
#include <string>
#include <memory>
#include <variant>
#include <cassert>
#include <expected>
#include <format>

namespace hk {
class source;
}

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

    [[nodiscard]] virtual hk::source& source() const
    {
        return top().source();
    }

    std::unexpected<hkc_error> add(hkc_error error) const
    {
        using namespace std::literals;
        return _add(error, ""s);
    }

    template<typename... Args>
    std::unexpected<hkc_error> add(hkc_error error, std::format_string<Args...> fmt, Args&&...args) const
    {
        return _add(error, std::format(std::move(fmt), std::forward<Args>(args)...));
    }

    [[nodiscard]] virtual generator<node *> children() const
    {
        co_return;
    }

    /** Evaluate conditional compilation expressions.
     * 
     * @param ctx The namespace used by the compilation expressions.
     */
    virtual std::expected<void, hkc_error> evaluate_build_guard(datum_namespace const& ctx);

    virtual char const *fixup(node *parent)
    {
        char const *child_last = nullptr;

        for (auto &node : children()) {
            assert(node != nullptr);
            if (auto p = node->fixup(this)) {
                child_last = p;
            }
        }

        _parent = parent;
        if (last == nullptr) {
            last = child_last;
        }
        return last;
    }

protected:
    node *_parent = nullptr;

private:
    std::unexpected<hkc_error> _add(hkc_error error, std::string message) const;

};

using node_ptr = std::unique_ptr<node>;

} // namespace hk::ast
