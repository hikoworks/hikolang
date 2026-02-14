
#pragma once

#include "om_object.hpp"

namespace hk {

/** A namespace holds different kind of objects by name.
 * 
 * The following type of objects can be contained by a namespace:
 *  - namespace
 *  - overload_set (function, type and global-variable templates)
 */
class om_namespace : public om_object {
public:

private:
    om_namespace *_parent = nullptr;
};

}
