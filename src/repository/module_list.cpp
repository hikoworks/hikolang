
#include "module_list.hpp"
#include "repository.hpp"
#include <compare>

namespace hk {

void module_list::add(source& s)
{
    if (s.enabled()) {
        return;
    }

    auto it = std::lower_bound(_sources.begin(), _sources.end(), s, [](auto const& item, auto const& x) {
        return cmp_names(*item, x) == std::strong_ordering::less;
    });

    if (it != _sources.end() and cmp_names(**it, s) == std::strong_ordering::equal) {
        if ((*it)->repository() == s.repository()) {
            // There should not be a module with the same name in the same repository.
            // This should have been handled by repository.cpp.
            std::terminate();
        }

        //if ((*it)->has_version() and s.has_version()) {
            // Two anchors are competing, best version will win.
        //}

    }
}

}