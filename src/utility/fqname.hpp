
#pragma once

#include <gsl/gsl>
#include <string>
#include <cassert>
#include <iterator>
#include <cstddef>
#include <algorithm>

namespace hk {

class fqname {
public:
    class const_iterator {
    public:
        struct proxy_type {
            std::string_view _s;

            constexpr std::string_view const *operator->() const noexcept
            {
                return std::addressof(_s);
            }
        };

        using iterator_category = std::input_iterator_tag;
        using value_type = std::string_view;
        using difference_type = std::ptrdiff_t;
        using pointer = const char*;
        using reference = const char&;

        constexpr const_iterator(const_iterator&) noexcept = default;
        constexpr const_iterator(const_iterator&&) noexcept = default;
        constexpr const_iterator& operator=(const_iterator&) noexcept = default;
        constexpr const_iterator& operator=(const_iterator&&) noexcept = default;

        /** Compares two iterators for equality.
         * 
         * Two iterators are considered equal if they point to the same component of the same fully qualified name.
         * 
         * @note It is UNDEFINED BEHAVIOR to compare iterators from different fully qualified names.
         * @param lhs The first iterator to compare.
         * @param rhs The second iterator to compare.
         * @return true if the iterators are equal, false otherwise.
         */
        [[nodiscard]] constexpr friend bool operator==(const_iterator const& lhs, const_iterator const& rhs) noexcept
        {
            assert(lhs._fq == rhs._fq);
            if (lhs._s.data() == rhs._s.data()) {
                assert(lhs._s.size() == rhs._s.size());
                return true;
            } else {
                return false;
            }
        }

        /** Compares an iterator with a default sentinel for equality.
         * 
         * An iterator is considered equal to the default sentinel if it points to the end of the fully qualified name.
         * 
         * @param lhs The iterator to compare.
         * @param rhs The default sentinel.
         * @return true if the iterator is equal to the default sentinel, false otherwise.
         */
        [[nodiscard]] constexpr friend bool operator==(const_iterator const& lhs, std::default_sentinel_t) noexcept
        {
            return lhs.pos() == lhs._fq->name().size();
        }

        /** Increment the iterator to point to the next component of the fully qualified name.
         * 
         * @note It is UNDEFINED BEHAVIOR to increment an iterator that is already at the end of the fully qualified name.
         * @return A reference to the incremented iterator.
         */
        constexpr const_iterator& operator++()
        {
            // Iterator can only be incremented if it is not at the end.
            assert(pos() < _fq->_name.size());

            fix_string_view(pos() + len() + 1);
            return *this;
        }

        /** Decrement the iterator to point to the previous component of the fully qualified name.
         *
         * @note It is UNDEFINED BEHAVIOR to decrement an iterator that is already at the beginning of the fully qualified name.
         * @return A reference to the decremented iterator.
         */
        constexpr const_iterator& operator--()
        {
            // Iterator can only be decremented if it is not at the beginning.
            assert(pos() > 0);

            auto const dot_pos = _fq->_name.rfind('.', pos() - 1);
            auto const new_pos = dot_pos != std::string::npos ? dot_pos + 1 : 0;
            fix_string_view(new_pos);
            return *this;
        }

        /** Postfix increment operator.
         *
         * Increments the iterator to point to the next component of the fully qualified name, returning the previous state.
         *
         * @note It is UNDEFINED BEHAVIOR to increment an iterator that is already at the end of the fully qualified name.
         * @return A copy of the iterator before incrementing.
         */
        [[nodiscard]] constexpr const_iterator operator++(int)
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        /** Postfix decrement operator.
         *
         * Decrements the iterator to point to the previous component of the fully qualified name, returning the previous state.
         *
         * @note It is UNDEFINED BEHAVIOR to decrement an iterator that is already at the beginning of the fully qualified name.
         * @return A copy of the iterator before decrementing.
         */
        [[nodiscard]] constexpr const_iterator operator--(int)
        {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        /** Dereference operator to access the current component of the fully qualified name.
         *
         * @note It is UNDEFINED BEHAVIOR to dereference an iterator that is at the end of the fully qualified name.
         * @return A reference to the current component of the fully qualified name.
         */
        [[nodiscard]] constexpr value_type operator*() const
        {
            assert(*this != std::default_sentinel);
            return _s;
        }

        /** Arrow operator to access the current component of the fully qualified name.
         *
         * @note It is UNDEFINED BEHAVIOR to dereference an iterator that is at the end of the fully qualified name.
         * @return A pointer to the current component of the fully qualified name.
         */
        [[nodiscard]] constexpr proxy_type operator->() const
        {
            assert(*this != std::default_sentinel);
            return proxy_type{_s};
        }

    private:
        fqname const* _fq = nullptr;
        value_type _s;

        constexpr const_iterator(fqname const &fq) noexcept
            : _fq(std::addressof(fq))
        {
            fix_string_view(0);
        }

        [[nodiscard]] constexpr size_t pos() const noexcept
        {
            return std::distance(_fq->_name.data(), _s.data());
        }

        [[nodiscard]] constexpr size_t len() const noexcept
        {
            return _s.size();
        }

        constexpr void fix_string_view(size_t new_pos) noexcept
        {
            auto const dot_pos = _fq->_name.find('.', new_pos);
            auto const new_len = dot_pos != std::string::npos ? dot_pos - new_pos : _fq->_name.size() - new_pos;

            _s = value_type{_fq->_name.data() + new_pos, new_len};
        }

        friend class fqname;
    };

    /** Construct an empty fully qualified name.
     */
    constexpr fqname() noexcept = default;

    constexpr fqname(fqname const&) noexcept = default;
    constexpr fqname(fqname&&) noexcept = default;
    constexpr fqname& operator=(fqname const&) noexcept = default;
    constexpr fqname& operator=(fqname&&) noexcept = default;

    constexpr fqname(std::string_view name)
    {
        auto n = 0uz;
        for (; n != name.size(); ++n) {
            if (name[n] != '.') {
                break;
            }
        }
        _num_prefix_dots = gsl::narrow<decltype(_num_prefix_dots)>(n);

        _name = name.substr(n);

        auto const dot_count = std::count(_name.begin(), _name.end(), '.');
        _num_components = gsl::narrow<decltype(_num_components)>(dot_count + 1);
    }

    constexpr fqname(std::string name)
        : fqname(std::string_view{name})
    {
    }


    constexpr fqname(char const* name)
        : fqname(std::string_view{name})
    {
    }

    [[nodiscard]] constexpr friend bool operator==(fqname const&, fqname const&) noexcept = default;

    /** Compares two fully qualified names lexicographically per component. 
     * 
     * @param lhs The first fully qualified name to compare.
     * @param rhs The second fully qualified name to compare.
     * @return A strong ordering indicating the comparison result.
     */
    [[nodiscard]] constexpr friend auto operator<=>(fqname const& lhs, fqname const& rhs) noexcept
    {
        auto it = lhs.begin();
        auto jt = rhs.begin();

        while (it != lhs.end() and jt != rhs.end()) {
            if (auto cmp = it->compare(*jt); cmp != 0) {
                return cmp < 0 ? std::strong_ordering::less : std::strong_ordering::greater;
            }
            ++it;
            ++jt;
        }

        if (it == lhs.end() and jt == rhs.end()) {
            return std::strong_ordering::equal;
        } else if (it == lhs.end()) {
            return std::strong_ordering::less;
        } else {
            return std::strong_ordering::greater;
        }
    }

    /** Get the fully qualified name as a string view. 
     * 
     */
    [[nodiscard]] constexpr std::string name() const noexcept
    {
        auto r = std::string{};
        r.reserve(_num_prefix_dots + _name.size());
        for (auto i = 0uz; i != _num_prefix_dots; ++i) {
            r += '.';
        }
        r += _name;
        return r;
    }

    constexpr operator std::string() const noexcept
    {
        return name();
    }

    /** Checks if this fully qualified name is a subname of another.
     * 
     *  A fully qualified name is a subname of another if:
     *  - Each component of both names matches in order.
     *  - The first name has no more components than the second name.
     * 
     *  @param other The other fully qualified name to check against.
     *  @return true if this name is a subname of the other, false otherwise
     */
    [[nodiscard]] constexpr bool is_subname_of(fqname const& other) const noexcept
    {
        auto it = this->begin();
        auto jt = other.begin();

        while (it != this->end() and jt != other.end()) {
            if (*it != *jt) {
                return false;
            }
            ++it;
            ++jt;
        }

        return it == this->end();
    }

    /** Get an iterator to the first component of the fully qualified name.
     * 
     *  @return An iterator to the first component of the fully qualified name.
     */
    [[nodiscard]] constexpr const_iterator begin() const noexcept
    {
        return const_iterator{*this};
    }

    /** Get a sentinel indicating the end of the fully qualified name.
     * 
     * @return A sentinel indicating the end of the fully qualified name.
     */
    [[nodiscard]] constexpr std::default_sentinel_t end() const noexcept
    {
        return std::default_sentinel;
    }

    /** Get the number of components in the fully qualified name.
     * 
     * @note O(n) complexity, where n is the length of the name.
     * @return The number of components in the fully qualified name.
     */
    [[nodiscard]] constexpr size_t size() const noexcept
    {
        return _num_components;
    }

    /** Checks if the fully qualified name is empty.
     *
     * @return true if the fully qualified name does not have components, false
     *         otherwise.
     */
    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return size() == 0;
    }

    [[nodiscard]] constexpr bool is_absolute() const noexcept
    {
        return _num_prefix_dots == 1;
    }

    [[nodiscard]] constexpr bool is_relative() const noexcept
    {
        return _num_prefix_dots == 0;
    }

    [[nodiscard]] constexpr bool is_canonical() const noexcept
    {
        return _num_prefix_dots <= 1;
    }

    constexpr void clear() noexcept
    {
        _name.clear();
        _num_components = 0;
        _num_prefix_dots = 0;
    }

    void set_num_prefix_dots(size_t n)
    {
        _num_prefix_dots = gsl::narrow<decltype(_num_prefix_dots)>(n);
    }

    /** Get the number of prefix dots.
     * 
     *  - 0: Name is relative to the current namespace
     *  - 1: Name is absolute to the root namespace
     *  - 2: Name backtracks one level up to the current namespace.
     *  - n: Name backtracks n levels up to the current namespace.
     * 
     */
    [[nodiscard]] constexpr size_t num_prefix_dots() const noexcept
    {
        return _num_prefix_dots;
    }

    /** Make an absolute path.
     *
     * @note It is UNDEFINED BEHAVIOUR if base is not an absolute path.
     * @param base Use the current namespace as the absolute name of which the
     *             @a this is relative to.
     * @return The absolute path by combining @a this with @a current_namespace
     * @retval empty An absolute could not be constructed.
     *
     */
    [[nodiscard]] constexpr fqname make_absolute(fqname const &base) const
    {
        assert(base.is_absolute());

        auto n = num_prefix_dots();
        if (n == 1) {
            return *this;
        }

        // prefix dots beyond 1 backtrack from the base name.
        if (n > 0) {
            --n;
        }

        auto r = base;
        for (; n != 0 and not r.empty(); --n) {
            r.pop_back();
        }
        if (n != 0) {
            // Can't construct absolute path.
            r.clear();
            return r;
        }

        for (auto i = 0uz; i != size(); ++i) {
            r += (*this)[i];
        }
        return r;
    }

    /** Get the component at the specified index.
     * 
     * @note O(n) complexity, where n is the index of the component.
     * @param i The index of the component to retrieve.
     * @return The component at the specified index.
     */
    [[nodiscard]] constexpr std::string_view operator[](size_t i) const
    {
        auto it = begin();
        for (size_t j = 0; j != i; ++j, ++it) {
            assert(it != end());
        }
        assert(it != end());
        return *it;
    }

    /** Append a component to the fully qualified name.
     * 
     * @note It is UNDEFINED BEHAVIOR to append an empty component or a component that contains a dot '.'.
     * @param component The component to append to the fully qualified name.
     */
    constexpr void operator+=(std::string_view component)
    {
        assert(not component.empty());
        assert(component.find('.') == std::string_view::npos);

        if (not _name.empty()) {
            _name += '.';
        }
        _name += component;

        assert(_num_components < std::numeric_limits<decltype(_num_components)>::max());
        ++_num_components;
    }

    /** Remove last component.
     * 
     * @note It is UNDEFINED BEHAVIOUR to pop from an empty fqname.
     */
    constexpr void pop_back()
    {
        assert(not empty());
        if (auto i = _name.rfind('.'); i == _name.npos) {
            _name.clear();
        } else {
            _name = _name.substr(0, i);
        }
    }

private:
    uint8_t _num_prefix_dots = 0;
    uint8_t _num_components = 0;
    std::string _name = {};
};

}
