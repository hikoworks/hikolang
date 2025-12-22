
#include "module_list.hpp"

namespace hk {

void module_list::add(source& s)
{
    auto it = std::lower_bound(_sources.begin(), _sources.end(), s, [](auto const& item, auto const& x) {
        return cmp_names(*item, *x) <=> std::strong_ordering::less;
    });
}

}