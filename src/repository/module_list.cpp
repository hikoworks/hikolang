
#include "module_list.hpp"
#include "repository.hpp"
#include <compare>
#include <unordered_set>

namespace hk {

void module_list::clear()
{
    _sources.clear();

#ifndef _NDEBUG
    _deduplicated = true;
    _marked_used = true;
#endif
}

void module_list::add(hk::source& source)
{
    if (to_bool(source.enabled())) {
        _sources.push_back(std::addressof(source));

#ifndef _NDEBUG
        _deduplicated = false;
        _marked_used = false;
#endif
    }
}

[[nodiscard]] source* module_list::find(fqname const& name) const
{
#ifndef _NDEBUG
    assert(_deduplicated);
#endif

    assert(name.is_absolute());

    auto it = std::lower_bound(_sources.begin(), _sources.end(), name, [](auto const& item, auto const& x) {
        if (item->kind() == source::kind_type::module) {
            return item->module_name() < x;
        } else {
            // Anything that is not a module is less.
            return true;
        }
    });

    if (it != _sources.end() and (*it)->kind() == source::kind_type::module and (*it)->module_name() == name) {
        return *it;
    }

    return nullptr;
}

void module_list::deduplicate()
{
    // cmp_names will sort on:
    //  * kind: program, library, module
    //  * module name declared in the prologue
    //  * repository path
    std::sort(_sources.begin(), _sources.end(), [](auto const& a, auto const& b) {
        return cmp_names(*a, *b) == std::strong_ordering::less;
    });

    auto anchor_stack = std::vector<source*>{};

    for (auto it = _sources.begin(); it != _sources.end(); ++it) {
        if (it != _sources.begin() and cmp_names(**(it - 1), **it) == std::strong_ordering::equal) {
            // Remove a fallback source if another source of the same name
            // exists. If there are two fallbacks, then they may remain and
            // cause a duplicate_module error.
            if ((*(it - 1))->enabled() == logic::_ and (*it)->enabled() == logic::T) {
                it = _sources.erase(it - 1);
            } else if ((*(it - 1))->enabled() == logic::T and (*it)->enabled() == logic::_) {
                it = _sources.erase(it) - 1;
            }
            
            // Two modules with the same name in the same repository.
            anchor_stack.back()->file_declaration().add(hkc_error::duplicate_module);
            (*it)->file_declaration().add(hkc_error::duplicate_module);

            // Keep only one of the duplicate anchors.
            // Adjust for the post-loop increment.
            it = _sources.erase(it) - 1;

        } else if ((*it)->version()) {
            // This is an anchor module.
            if (not anchor_stack.empty() and anchor_stack.back()->module_name() == (*it)->module_name()) {
                assert(it != _sources.begin());
                assert(*(it - 1) == anchor_stack.back());

                if (cmp_versions(*anchor_stack.back(), **it) == std::strong_ordering::less) {
                    // The previous anchor had a lower version, remove it and replace with the new anchor.
                    it = _sources.erase(it - 1);
                    anchor_stack.back() = *it;

                } else {
                    // The current anchor has a lower version, remove it.
                    // Adjust for the post-loop increment.
                    it = _sources.erase(it) - 1;
                }

            } else {
                // The anchor stack should only contain parent anchors, pop
                // until this is true.
                while (not anchor_stack.empty() and not is_child_of(**it, *anchor_stack.back())) {
                    anchor_stack.pop_back();
                }
                anchor_stack.push_back(*it);
            }

        } else if (not anchor_stack.empty() and is_child_of(**it, *anchor_stack.back())) {
            // A sub-module.
            if (anchor_stack.back()->repository() != (*it)->repository()) {
                // This sub-module is not part of the repository that has the
                // winning anchor. Adjust for the post-loop increment.
                it = _sources.erase(it) - 1;
            }

        } else {
            // A unanchored sub-module
            (*it)->file_declaration().add(hkc_error::missing_anchor_module);
            // Remove the module without an anchor. Adjust for post-loop
            // increment.
            it = _sources.erase(it) - 1;
        }
    }

#ifndef _NDEBUG
    _deduplicated = true;
#endif
}

void module_list::mark_used(std::vector<source*> todo)
{
    // Clear the used flag on all sources.
    for (auto source : _sources) {
        source->set_used(false);
    }

    auto done = std::unordered_set<source*>{};
    while (not todo.empty()) {
        // Move entry from todo to done.
        auto source = todo.back();
        todo.pop_back();
        done.insert(source);

        source->set_used(true);

        for (auto import_declaration : source->imported_modules()) {
            if (auto imported_module_ptr = find(import_declaration->name)) {
                // Add the imported module to the todo list.
                if (done.find(imported_module_ptr) == done.end()) {
                    todo.push_back(imported_module_ptr);
                }

            } else {
                import_declaration->add(hkc_error::imported_module_not_found);
            }
        }
    }

    _marked_used = true;
}

} // namespace hk