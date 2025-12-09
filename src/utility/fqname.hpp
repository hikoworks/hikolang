
#pragma once

#include <gsl/gsl>
#include <string>
#include <cassert>
#include <iterator>
#include <cstddef>
#include <algorithm>
#include <string_view>
#include <memory>
#include <bit>
#include <compare>

namespace hk {

/** A fully qualified name.
 * 
 * Limitations:
 *  - Total size of components 255
 *  - Total number of components 16
 *  - Total number of prefix dots 16
 * 
 */
class fqname {
public:
    using value_type = std::string_view;
    using allocator_type = std::allocator<char>;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using const_reference = std::string_view;

    class const_iterator {
    public:
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using value_type = std::string_view;

        struct proxy_pointer {
            std::string_view value;

            constexpr proxy_pointer() noexcept = default;
            constexpr proxy_pointer(proxy_pointer const&) noexcept = default;
            constexpr proxy_pointer(proxy_pointer&&) noexcept = default;
            constexpr proxy_pointer& operator=(proxy_pointer const&) noexcept = default;
            constexpr proxy_pointer& operator=(proxy_pointer&&) noexcept = default;
            constexpr proxy_pointer(std::string_view other) noexcept : value(other) {}

            [[nodiscard]] constexpr std::string_view const *operator->() const noexcept
            {
                return std::addressof(value);
            }
        };

        constexpr const_iterator() noexcept = default;
        constexpr const_iterator(const_iterator const&) noexcept = default;
        constexpr const_iterator(const_iterator&&) noexcept = default;
        constexpr const_iterator& operator=(const_iterator const&) noexcept = default;
        constexpr const_iterator& operator=(const_iterator&&) noexcept = default;

        constexpr const_iterator(fqname const *self, size_t i) : self(self), i(i)
        {
            assert(self != nullptr);
            assert(i <= self->size());
        }

        [[nodiscard]] constexpr friend bool operator==(const_iterator const& lhs, const_iterator const& rhs)
        {
            assert(lhs.self == rhs.self);
            return lhs.i == rhs.i;
        }

        [[nodiscard]] constexpr friend auto operator<=>(const_iterator const& lhs, const_iterator const& rhs)
        {
            assert(lhs.self == rhs.self);
            return lhs.i <=> rhs.i;
        }

        [[nodiscard]] constexpr std::string_view operator*() const
        {
            assert(self != nullptr);
            return (*self)[i];
        }
        
        [[nodiscard]] constexpr proxy_pointer operator->() const
        {
            return proxy_pointer{**this};
        }

        [[nodiscard]] constexpr const_iterator &operator++()
        {
            ++i;
            return *this;
        }

        [[nodiscard]] constexpr const_iterator operator++(int)
        {
            auto tmp = *this;
            ++i;
            return tmp;
        }

    private:
        fqname const *self = nullptr;
        size_t i = 0;
    };

    static_assert(std::allocator_traits<allocator_type>::is_always_equal::value);

    constexpr ~fqname()
    {
        if (_capacity != 0) {
            auto allocator = allocator_type{};
            std::allocator_traits<allocator_type>::deallocate(allocator, _ptr, _capacity);
        }
    }

    /** Create empty fully qualified name.
     */
    constexpr fqname() noexcept : _buffer1(), _buffer2(), _offsets(), _size(0), _prefix(0), _capacity(0)
    {
        std::fill_n(&_buffer1[0], _buffer1_size, 0);
        std::fill_n(&_buffer2[0], _buffer2_size, 0);
        std::fill_n(&_offsets[0], _offsets_size, 0);
    }

    /** Copy a fully qualified name.
     */
    constexpr fqname(fqname const &other)
    {
        if (other._capacity != 0) {
            auto allocator = allocator_type{};
            _ptr = std::allocator_traits<allocator_type>::allocate(allocator, other._capacity);
            std::copy_n(other._ptr, other._capacity, _ptr);
        } else {
            std::copy_n(&other._buffer1[0], _buffer1_size, &_buffer1[0]);
        }
        std::copy_n(&other._buffer2[0], _buffer2_size, &_buffer2[0]); 
        std::copy_n(&other._offsets[0], _offsets_size, &_offsets[0]); 

        _size = other._size;
        _prefix = other._prefix;
        _capacity = other._capacity;
    }

    /** Move a fully qualified name.
     */
   constexpr fqname(fqname&& other)
    {
        if (other._capacity != 0) {
            _ptr = other._ptr;
            std::fill_n(&other._buffer1[0], _buffer1_size, 0); 
        } else {
            std::copy_n(&other._buffer1[0], _buffer1_size, &_buffer1[0]);
        }
        std::copy_n(&other._buffer2[0], _buffer2_size, &_buffer2[0]); 
        std::copy_n(&other._offsets[0], _offsets_size, &_offsets[0]); 

        _size = other._size;
        _prefix = other._prefix;
        _capacity = other._capacity;
        other._size = 0;
        other._prefix = 0;
        other._capacity = 0;
    }

    /** Copy-assign a fully qualified name.
     */
    constexpr fqname& operator=(fqname const& other)
    {
        if (this == &other) {
            return *this;
        }

        auto allocator = allocator_type{};
        if (other._capacity != 0) {
            if (_capacity < other._capacity) {
                if (_capacity != 0) {
                    std::allocator_traits<allocator_type>::deallocate(allocator, _ptr, _capacity);
                }

                _ptr = std::allocator_traits<allocator_type>::allocate(allocator, other._capacity);
                _capacity = other._capacity;
            }
            std::copy_n(other._ptr, other._capacity, _ptr);

        } else {
            if (_capacity != 0) {
                std::allocator_traits<allocator_type>::deallocate(allocator, _ptr, _capacity);
            }
            std::copy_n(&other._buffer1[0], _buffer1_size, &_buffer1[0]);
            _capacity = 0;
        }

        std::copy_n(&other._buffer2[0], _buffer2_size, &_buffer2[0]);
        std::copy_n(&other._offsets[0], _offsets_size, &_offsets[0]);
        _size = other._size;
        _prefix = other._prefix;
        return *this;
    }

    /** Move-assign a fully qualified name.
     */
    constexpr fqname& operator=(fqname&& other)
    {
        if (this == &other) {
            return *this;
        }

        auto allocator = allocator_type{};
        if (_capacity != 0) {
            std::allocator_traits<allocator_type>::deallocate(allocator, _ptr, _capacity);
        }

        if (other._capacity != 0) {
            _ptr = other._ptr;
            std::fill_n(&other._buffer1[0], _buffer1_size, 0); 
        } else {
            std::copy_n(&other._buffer1[0], _buffer1_size, &_buffer1[0]);
        }
        std::copy_n(&other._buffer2[0], _buffer2_size, &_buffer2[0]); 
        std::copy_n(&other._offsets[0], _offsets_size, &_offsets[0]); 

        _size = other._size;
        _prefix = other._prefix;
        _capacity = other._capacity;
        other._size = 0;
        other._prefix = 0;
        other._capacity = 0;
        return *this;
    }

    /** Create a fully qualified name from a string.
     * 
     * @throws std::invalid_argument With too many prefix dots.
     */
    constexpr fqname(std::string_view str) : fqname()
    {
        auto i = 0uz;
        while (i != str.size()) {
            auto j = str.find('.', i);
            j = std::min(j, str.size());

            append_component(str.substr(i, j));
            i = j;
        }
    }

    [[nodiscard]] constexpr friend bool operator==(fqname const& lhs, fqname const& rhs) noexcept
    {
        if (lhs._prefix != rhs._prefix) {
            return false;
        }

        if (not std::ranges::equal(lhs.get_span(), rhs.get_span())) {
            return false;
        }

        assert(lhs.size() == rhs.size());
        for (auto i = 0uz; i != lhs.size(); ++i) {
            if (lhs.get_offset(i + 1) != rhs.get_offset(i + 1)) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] constexpr friend auto operator<=>(fqname const& lhs, fqname const& rhs) noexcept
    {
        if (auto r = lhs._prefix <=> rhs._prefix; r != std::strong_ordering::equal) {
            return r;
        }

        return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    [[nodiscard]] constexpr size_t prefix() const noexcept
    {
        return _prefix;
    }

    [[nodiscard]] constexpr bool is_absolute() const noexcept
    {
        return prefix() == 1;
    }
    
    [[nodiscard]] constexpr bool is_relative() const noexcept
    {
        return prefix() != 1;
    }

    [[nodiscard]] constexpr size_t size() const noexcept
    {
        return _size;
    }
    
    [[nodiscard]] constexpr const_iterator begin() const noexcept
    {
        return const_iterator{this, 0uz};
    }

    [[nodiscard]] constexpr const_iterator end() const noexcept
    {
        return const_iterator{this, size()};
    }

    constexpr void clear()
    {
        if (_capacity != 0) {
            auto allocator = allocator_type{};
            std::allocator_traits<allocator_type>::deallocate(allocator, _ptr, _capacity);
        }
        _prefix = 0;
        _size = 0;
        _capacity = 0;
    }

    /** Get the capacity in bytes.
     */
    [[nodiscard]] constexpr size_t capacity() const noexcept
    {
        if (_capacity) {
            return _capacity;
        } else {
            return _internal_space - size() - 1;
        }
    }

    constexpr void reserve(size_t cap)
    {
        assert(cap < 256);

        if (cap <= capacity()) {
            return;
        }

        cap = std::bit_ceil(cap);

        auto allocator = allocator_type{};
        auto const ptr = std::allocator_traits<allocator_type>::allocate(allocator, cap);
        
        auto old_span = get_span();
        std::copy(old_span.begin(), old_span.end(), ptr); 

        _ptr = ptr;
        _capacity = cap;
    }

    constexpr fqname& append_component(std::string_view component)
    {
        assert(_size < 15);

        auto const last_offset = get_offset(size()) + component.size();
        reserve(last_offset);

        auto s = get_span(size(), component.size());
        std::copy(component.begin(), component.end(), s.begin());
        ++_size;
        set_offset(size(), last_offset);

        return *this;
    }

    constexpr fqname& pop_component()
    {
        assert(_size != 0);
        --_size;
        return *this;
    }

    constexpr fqname& operator/=(fqname const& rhs)
    {
        for (auto i = 0uz; i != std::min(rhs._prefix, _size); ++i) {
            pop_component();
        }

        for (auto i = 0uz; i != rhs._size; ++i) {
            append_component(rhs[i]);
        }

        return *this;
    }

    constexpr fqname& operator/=(std::string_view str)
    {
        return *this /= fqname{str};
    }

    [[nodiscard]] constexpr fqname operator/(fqname const& rhs) const
    {
        auto tmp = *this;
        return tmp /= rhs;
    }

    [[nodiscard]] constexpr fqname operator/(std::string_view str) const
    {
        return *this / fqname{str};
    }

    [[nodiscard]] constexpr std::string_view operator[](size_t i) const
    {
        return std::string_view(get_span(i));
    }

    [[nodiscard]] constexpr std::string_view first() const
    {
        return (*this)[0];
    }

    [[nodiscard]] constexpr std::string_view last() const
    {
        return (*this)[size() - 1];
    }

    [[nodiscard]] constexpr std::string string() const
    {
        auto r = std::string{};
        if (_capacity) {
            r.reserve(_prefix + get_offset(size()) + _size);
        }

        for (auto i = 0uz; i != _prefix; ++i) {
            r += '.';
        }
        
        for (auto i = 0uz; i != _size; ++i) {
            if (i != 0uz) {
                r += '.';
            }
            r += (*this)[i];
        }

        return r;
    }

private:
    constexpr static size_t _this_size = 32uz;
    constexpr static size_t _offsets_size = 16uz;
    constexpr static size_t _buffer1_size = sizeof(uint8_t *);
    constexpr static size_t _buffer2_size = _this_size - 2 - _offsets_size - _buffer1_size;
    constexpr static size_t _internal_space = _buffer1_size + _buffer2_size + _offsets_size;

    // 256 bits.
    union {
        char *_ptr;
        char _buffer1[_buffer1_size];
    };
    char _buffer2[_buffer2_size];
    char _offsets[_offsets_size];
    uint8_t _prefix : 4 = 0;
    uint8_t _size : 4 = 0;
    uint8_t _capacity = 0;

    [[nodiscard]] constexpr size_t get_offset(size_t i) const noexcept
    {
        assert(i < _size + 1);
        if (i == 0) {
            return 0;
        } else {
            return static_cast<uint8_t>(_offsets[_offsets_size - i]);
        }
    }

    constexpr void set_offset(size_t i, size_t offset) noexcept
    {
        assert(i < _size + 1);
        assert(i != 0);
        _offsets[_offsets_size - i] = static_cast<char>(gsl::narrow<uint8_t>(offset));
    }

    [[nodiscard]] constexpr char *get_char_pointer(size_t i) noexcept
    {
        if (_capacity == 0) {
            return &_buffer1[get_offset(i)];
        } else {
            return &_ptr[get_offset(i)];
        }
    }

    [[nodiscard]] constexpr char const *get_char_pointer(size_t i) const noexcept
    {
        if (_capacity == 0) {
            return &_buffer1[get_offset(i)];
        } else {
            return &_ptr[get_offset(i)];
        }
    }

    [[nodiscard]] constexpr std::span<char> get_span() noexcept
    {
        return {get_char_pointer(0), get_char_pointer(size())};
    }

    [[nodiscard]] constexpr std::span<char const> get_span() const noexcept
    {
        return {get_char_pointer(0), get_char_pointer(size())};
    }

    [[nodiscard]] constexpr std::span<char> get_span(size_t i) noexcept
    {
        assert(i < size());
        return {get_char_pointer(i), get_char_pointer(i + 1)};
    }

    [[nodiscard]] constexpr std::span<char const> get_span(size_t i) const noexcept
    {
        assert(i < size());
        return {get_char_pointer(i), get_char_pointer(i + 1)};
    }

    [[nodiscard]] constexpr std::span<char> get_span(size_t i, size_t size) noexcept
    {
        assert(i < this->size() + 1);
        return {get_char_pointer(i), size};
    }

};


}
