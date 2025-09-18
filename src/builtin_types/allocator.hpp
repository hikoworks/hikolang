
#pragma once

#include <utility>
#include <cstdlib>
#include <memory>

namespace hk::builtin {

class allocator_intf {
public:
    virtual ~allocator_intf() = default;
    
    /** Allocate memory.
     * 
     * @param size The minimum size of memory to allocate.
     * @param alignment The alignment of the memory to allocate.
     * @return The pointer to the allocated memory, the actual size of the allocated memory.
     */
    [[nodiscard]] virtual std::pair<void *, std::size_t> allocate(std::size_t size, std::size_t alignment) = 0;

    /** Deallocate memory.
     * 
     * @param ptr The location of allocated memory.
     * @param size The actual size of the allocated memory.
     */
    virtual void deallocate(void *ptr, std::size_t size) = 0;
};

class default_allocator : public allocator_intf {
public:
    virtual ~default_allocator() = default;

    default_allocator()
    {
        _allocator = std::default_allocator{};
    }

    [[nodiscard]] std::pair<void *, std::size_t> allocate(std::size_t size, std::size_t alignment) override
    {
        auto [ptr, actual_size] _allocator.allocate_at_least(size);
        return {static_cast<void *>(ptr), actual_size};
    }

    void deallocate(void *ptr, std::size_t size) override
    {
        _allocator.deallocate(ptr, size);
    }

private:
    std::default_allocator<char> _allocator;
}

inline default_allocator _default_allocator = default_allocator{};

[[nodiscard]] std::pair<void *, std::size_t> allocate(std::size_t size, std::size_t alignment)
{
    return _default_allocator(size, alignment);
}

void deallocate(void *ptr, std::size_t size)
{
    return _default_allocator(ptr, size);
}

}