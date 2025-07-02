
#pragma once

#include <string>
#include <cassert>
#include <iterator>
#include <cstddef>

namespace hl {

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

    /** Constructs a fully qualified name from a string.
     * 
     *  The string should be in the format of a fully qualified name, with components
     *  separated by dots '.'. For example, "com.github.hikolang".
     * 
     * @param name The string representing the fully qualified name.
     */
    constexpr fqname(std::string name) noexcept
        : _name(std::move(name))
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
    [[nodiscard]] constexpr std::string_view name() const noexcept
    {
        return _name;
    }

    constexpr operator std::string_view() const noexcept
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

    /** Checks if the fully qualified name is empty.
     *
     * @return true if the fully qualified name does not have components, false
     *         otherwise.
     */
    [[nodiscard]] constexpr bool empty() const noexcept
    {
        return _name.empty();
    }

    /** Get the number of components in the fully qualified name.
     * 
     * @note O(n) complexity, where n is the length of the name.
     * @return The number of components in the fully qualified name.
     */
    [[nodiscard]] constexpr size_t size() const noexcept
    {
        if (_name.empty()) {
            return 0;
        }

        auto count = 1uz;
        for (auto c :  _name) {
            if (c == '.') {
                ++count;
            }
        }

        return count;
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
    }

private:
    std::string _name = {};
};

}
