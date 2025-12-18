
#pragma once

#include "source.hpp"
#include <vector>

namespace hk {

class module_list {
public:
    void add(source& s);

private:
    std::vector<source *> _sources;
};


}