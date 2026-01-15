
#pragma once

#include "symbol.hpp"
#include <stdint.h>

namespace hk::builtin {

using int_reg_t = intptr_t;

struct builtin_long : public symbol_type {
    union {
        int_reg_t value = 0;
        int_reg_t *ptr;
    };
    uint32_t size = 0;
    uint32_t capacity = 0;
};

}
