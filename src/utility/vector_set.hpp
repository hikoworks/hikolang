
#pragma once

#include <vector>
#include <algorithm>

namespace hk {

template <typename T>
class vector_set {
public:
    /** Add a value to the set.
     * 
     * If the value is already present, it will not be added again.
     * 
     * @param value The value to add.
     * @return `true` if the value was added, `false` if it was already present.
     */
    constexpr bool add(const T& value) {
        auto const it = std::lower_bound(_data.begin(), _data.end(), value);
        if (it == _data.end() or *it != value) {
            _data.insert(it, value);
            return true;
        } else {
            return false;
        }
    }

    /** Check if the set contains a value.
     * 
     * @param value The value to check for.
     * @return `true` if the value is present, `false` otherwise.
     */
    [[nodiscard]] constexpr bool contains(const T& value) const {
        auto const it = std::lower_bound(_data.begin(), _data.end(), value);
        return it != _data.end() and *it == value;
    }

private:
    std::vector<T> _data;
};


}
