
#pragma once

#include "utility/vector_map.hpp"
#include <memory>

namespace hk {

/** Base object.
 * 
 */
class om_object {
public:
    virtual ~om_object() = default;
    om_object(om_object const&) = delete;
    om_object(om_object&&) = delete;
    om_object& operator=(om_object const&) = delete;
    om_object& operator=(om_object&&) = delete;
    om_object() = default;


private:
    om_object *_parent = nullptr;

    /** The dictionary of the object
     */
    vector_map<std::string, std::shared_ptr<om_object>> _dict;
};

}
