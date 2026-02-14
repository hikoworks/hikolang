
#pragma once

#include "vector_map.hpp"

namespace hk {

/** A set based on a sorted std::vector
 * 
 * The vector maintains a list of sorted items for quick searching.
 * 
 * New items are appended to the vector, a duplicate key check is
 * performed on both the sorted and unsorted items in the vector.
 * 
 * @tparam K Type of the key
 */
template <typename K>
class vector_set : public vector_map<K, void> {
public:
    using vector_map<K, void>::vector_map;

};

}
