
#include "module_list.hpp"
#include "repository.hpp"
#include <compare>

namespace hk {

void module_list::add(source& s)
{
    if (s.enabled() and s.kind() == source::kind_type::module) {
        _sources.push_back(std::addressof(s));
        _fixed = false;
    }
}

[[nodiscard]] source* module_list::find(fqname const& name) const
{
    if (not _fixed) {
        const_cast<module_list*>(this)->fixup();
        assert(_fixed);
    }

    auto it = std::lower_bound(_sources.begin(), _sources.end(), name, [](auto const& item, auto const& x) {
        return item->module_name() < x;
    });

    if (it != _sources.end() and (*it)->module_name() == name) {
        return *it;
    }

    return nullptr;
}

void module_list::fixup()
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
                if (cmp_versions(**(it - 1), **it) == std::strong_ordering::less) {
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

    _fixed = true;
}

} // namespace hk