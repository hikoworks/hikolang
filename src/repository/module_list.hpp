
#pragma once

#include "source.hpp"
#include <vector>

namespace hk {

/** The list of modules in the project.
 * 
 * This holds the list of all module files.
 */
class module_list {
public:
    void clear();

    /** Add a source to the module-list.
     *
     * @note Filters away disabled or non-module sources.
     * @param source The source to add.
     */
    void add(hk::source& source);

    /** Find a module by name.
     * 
     * @param name The name of the module to find.
     * @return A pointer to the source.
     * @retval nullptr module was not found.
     */
    [[nodiscard]] source *find(fqname const& name) const;

    /** Deduplicate the set of modules.
     * 
     * Tasks:
     *   * Order the modules by name for binary-search.
     *   * Report duplicate-modules in the same repository.
     *   * Remove all sub-modules under an anchor, if that anchor has a lower
     *     version than a duplicate anchor in another repository.
     *   * Handle the fact that sub-anchors may exist in the same or different
     *     repositories.
     */
    void deduplicate();

    /** Mark all the modules that are used in this project.
     * 
     * @param todo A list of sources that must be compiled.
     */
    void mark_used(std::vector<source *> todo);

private:
    std::vector<source *> _sources;

#ifndef _NDEBUG
    bool _deduplicated = true;
    bool _marked_used = true;
#endif
};


}