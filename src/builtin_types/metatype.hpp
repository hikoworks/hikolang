
#pragma once

#include <cstddef>

namespace hk::builtin {

class metatype {
public:
    virtual ~metatype() = default;

    /** Number of bytes to allocate for an instance of the type it describes.
     */
    [[nodiscard]] virtual std::size_t size()
    {
        return 0;
    }

    /** Alignment in memory for an instance of type it describes.
     */
    [[nodiscard]] virtual std::size_t alignment()
    {
        return 1;
    }
};


}