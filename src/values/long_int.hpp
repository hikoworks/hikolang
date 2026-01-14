
#pragma once

#include <cstdint>
#include "allocator.hpp"

namespace hk::builtin {

using int_reg_t = std::int64_t;
using uint_reg_t = std::uint64_t;

struct long_int {
    union {
        uint_reg_t *pointer;
        std::int64_t value = 0;
    };
    uint32_t size = 0;
    uint32_t capacity = 0;

    ~long_int() {
        if (size != 0) {
            deallocate(pointer, capacity);
        }
    }

    constexpr long_int() noexcept = default;

    constexpr long_int(long_int const& rhs) {
        if (rhs.capacity == 0) {
            value = rhs.value;
            size = 0;
            capacity = 0;
        } else {
            auto [tmp_ptr, tmp_capacity] = allocate(rhs.size, sizeof(uint_reg_t));
            pointer = static_cast<uint_reg_t *>(tmp_ptr);
            size = rhs.size;
            capacity = tmp_capacity;
            memcpy
        }
    }

};

}