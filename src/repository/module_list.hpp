
#pragma once

#include "source.hpp"
#include <vector>

namespace hk {

class module_list {
public:
    /** Add a source to the module-list.
     */
    void add(source& s);


    [[nodiscard]] source *find(fqname const& name) const;

private:
    std::vector<source *> _sources;
    bool _fixed = true;

    /** Fixup the set of modules.
     * 
     * Tasks:
     *   * Order the modules by name for binary-search.
     *   * Report duplicate-modules in the same repository.
     *   * Remove all sub-modules under an anchor, if that anchor has a lower
     *     version than a duplicate anchor in another repository.
     *   * Handle the fact that sub-anchors may exist in the same or different
     *     repositories.
     */
    void fixup();
};


}