
#pragma once

#include "utility/file_location.hpp"

namespace hl::ast {

class node {
public:
    /** The location in the file where the first character of a node is located.
     */
    file_location first = {};

    /** One position after the location of the last character of the node.
     */
    file_location last = {};

    virtual ~node() = default;
    constexpr node(file_location first) noexcept : first(first), last(first) {}
    constexpr node(file_location first, file_location last) noexcept : first(first), last(last) {}

};

}
