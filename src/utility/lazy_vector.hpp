
#pragma once

#include <vector>
#include <iterator>
#include <compare>

namespace hk {

/** A lazy vector, lazilly copies data from an iterator.
 *
 * The iterators of the lazy_vector are not invalidated when
 * it is growing.
 */
template<typename It, typename ItEnd, typename T>
class lazy_vector {
public:
    using value_type = T;

    class const_iterator {
    public:
        constexpr const_iterator() = default;
        constexpr const_iterator(lazy_vector const *p, size_t i = 0uz) : _p(p), _i(i)
        {
            assert(_p != nullptr);
            _p->advance(i);
        }

        [[nodiscard]] constexpr value_type const& operator*() const
        {
            assert(_p != nullptr);
            if (_i < _p->_v.size()) {
                return _p->_v[_i];
            } else {
                assert(_p->_it == _p->_last);
                return _p->_default;
            }
        }

        [[nodiscard]] constexpr value_type const* operator->() const
        {
            assert(_p != nullptr);
            if (_i < _p->_v.size()) {
                return &_p->_v[_i];
            } else {
                assert(_p->_it == _p->_last);
                return &_p->_default;
            }
        }

        [[nodiscard]] constexpr value_type const& operator[](ptrdiff_t i) const
        {
            auto tmp = _i;
            if (i >= 0) {
                tmp += i;
            } else {
                i = -i;
                assert(i <= tmp);
                tmp -= i;
            }

            assert(_p != nullptr);
            return (*_p)[tmp];
        }

        [[nodiscard]] constexpr bool operator==(std::default_sentinel_t) const noexcept
        {
            return _p == nullptr or _i >= _p->_v.size();
        }

        [[nodiscard]] constexpr friend bool operator==(const_iterator const& lhs, const_iterator const& rhs) noexcept
        {
            if (lhs._p == nullptr and rhs._p == nullptr) {
                return true;
            } else if (lhs._p != rhs._p) {
                return false;
            } else {
                return lhs._p->_i == rhs._p->_i;
            }
        }

        [[nodiscard]] constexpr friend std::partial_ordering operator<=>(const_iterator const& lhs, const_iterator const& rhs) noexcept
        {
            if (lhs._p == nullptr and rhs._p == nullptr) {
                return std::partial_ordering::equivalent;
            } else if (lhs._p != rhs._p) {
                return std::partial_ordering::unordered;
            } else {
                return lhs._p->_i <=> rhs._p->_i;
            }
        }
        
        const_iterator& operator++()
        {
            assert(_p != nullptr);
            _p->advance(++_i);
            return *this;
        }
        
        const_iterator operator++(int)
        {
            auto tmp = this;
            ++(*this);
            return tmp;
        }

        const_iterator& operator+=(ptrdiff_t more)
        {
            if (more == 0) {
                return *this;
            } else if (more < 0) {
                return *this -= -more;
            } else {
                assert(_p != nullptr);
                _p->advance(_i += more);
                return *this;
            }
        }

        const_iterator& operator--()
        {
            assert(_p != nullptr);
            assert(_i != 0);
            --_i;
            return *this;
        }

        const_iterator operator--(int)
        {
            auto tmp = this;
            --(*this);
            return tmp;
        }

        const_iterator& operator-=(ptrdiff_t less)
        {
            if (less == 0) {
                return *this;
            } else if (less < 0) {
                return *this += -less;
            } else {
                assert(_i <= less);
                _i -= less;
                return *this;
            }
        }


    private:
        lazy_vector const *_p = nullptr;
        size_t _i = 0;
    };

    constexpr lazy_vector() = default;
    constexpr lazy_vector(It first, ItEnd last) : _it(first), _last(last) {}

    /** Get the value at index.
     * 
     * @param index The index into the vector. If the index is larger than
     *              the full expansion, the default constructed`value_type{}`
     *              is returned.
     * @return A const reference to the value pointed to by @a index.
     */
    [[nodiscard]] constexpr value_type const& operator[](size_t index) const
    {
        advance(index);

        if (index < _v.size()) {
            return _v[index];
        } else {
            return _default;
        }
    }

    [[nodiscard]] constexpr const_iterator begin() const
    {
        return {this};
    }

    [[nodiscard]] constexpr const_iterator cbegin() const
    {
        return {this};
    }

    [[nodiscard]] constexpr std::default_sentinel_t end() const
    {
        return std::default_sentinel;
    }

    [[nodiscard]] constexpr std::default_sentinel_t cend() const
    {
        return std::default_sentinel;
    }

private:
    mutable std::vector<value_type> _v;
    mutable It _it;
    ItEnd _last;
    value_type _default = {};

    /** Advance the internal copying until the index is available for read.
     * 
     * @param i Copy from the iterator until @a i is available as an index.
     */
    void advance(size_t i) const
    {
        while (i >= _v.size() and _it != _last) [[unlikely]] {
            _v.push_back(*_it);
            ++_it;
        }
    }

    friend const_iterator;
};

template<typename It, typename ItEnd>
lazy_vector(It first, ItEnd last) -> lazy_vector<It, ItEnd, std::iter_value_t<It>>; 

} // namespace hk
