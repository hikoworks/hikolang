
#pragma once

#include "tokenizer/file_location.hpp"
#include <string>
#include <memory>

namespace hk::ast {

class node {
public:
    /** The location in the file where the first character of a node is located.
     */
    file_location first = {};

    /** One position after the location of the last character of the node.
     */
    file_location last = {};

    /** Description of the node.
     */
    std::string description = {};

    virtual ~node() = default;
    constexpr node(file_location first) noexcept : first(first), last(first) {}
    constexpr node(file_location first, file_location last) noexcept : first(first), last(last) {}

};

using node_ptr = std::unique_ptr<node>;

}
