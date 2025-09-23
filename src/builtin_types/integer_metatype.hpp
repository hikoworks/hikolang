
#pragma once

#include "metatype.hpp"
#include <cstddef>
#include <cstdint>

namespace hk::builtin {

class integer_metatype : public metatype {
public:
    virtual ~integer_metatype() = default;

    [[nodiscard]] std::size_t size() override
    {
        return (_num_bits + 7) / 8;
    }

    [[nodiscard]] std::size_t alignment() override
    {
        return size();
    }

private:
    std::uint8_t _num_bits = 0;
    bool _signed = false;
};

}