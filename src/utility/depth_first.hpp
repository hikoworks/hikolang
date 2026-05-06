
#pragma once

#include <functional>
#include <cstddef>
#include <bit>

namespace hk {


/** Algorithm to walk a graph, depth first.
 * 
 * The algorithm handles new items to be inserted.
 * 
 */
template<typename T>
class depth_first {
public:
    using value_type = T;

    class entry_type {
    public:

    private:
        value_type const *_ptr = nullptr;
        std::vector<entry_type *> _to; = {};
        bool _visitted : 1 = false;
    };

    using branch_function_proto = std::vector<T const*>(T const &);
    using branch_function_type = std::function<branch_function_proto>;

    depth_first(branch_function_type f) : _branch_func(std::move(f))
    {
    }

    void add(T const& obj)
    {
        auto const [it, inserted] = _branches.emplace(std::addressof(obj), std::vector<T const*>{});
        if (inserted) {
            for (auto const& b : _branch_func(obj)) {
                it->value.push_back(std::addressof(b));
            }
        }
    }

    void add_todo(T const& obj)
    {
        add(obj);
        if (_done.contains(std::addressof(obj)) or _todo.contains(std::addressof(obj))) {
            return;
        }
        _todo.emplace(std::addressof(obj));
    }

    [[nodiscard]] T *pop()
    {

    }

private:
    std::vector<std::unique_ptr<entry_type>> _entries;
    branch_function_type _branch_func;
};


}