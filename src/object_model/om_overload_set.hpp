
#pragma once

#include "om_object.hpp"

namespace hk {

class om_overload_set : public om_object {
public:

private:
    std::vector<std::unique_ptr<om_template>> _templates;
};


}