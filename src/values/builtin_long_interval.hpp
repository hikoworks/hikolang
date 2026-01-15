
#pragma once

#include "long.hpp"

namespace hk::builtin {

struct long_interval : public symbol_type {
    builtin_long lo;
    builtin_long hi;
};


}