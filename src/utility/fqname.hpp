
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
 *
 */
class fqname {
public:
    class const_iterator {
    public:
        using value_type = std::string_view;

        class pointer {
        public:
            constexpr pointer(std::string_view other) noexcept : _value(other) {}

            [[nodiscard]] constexpr std::string_view const* operator->() const noexcept
            {
                return std::addressof(_value);
            }

        private:
            std::string_view _value;
        };

        constexpr const_iterator() noexcept = default;
        constexpr const_iterator(const_iterator const&) noexcept = default;
        constexpr const_iterator(const_iterator&&) noexcept = default;
        constexpr const_iterator& operator=(const_iterator const&) noexcept = default;
        constexpr const_iterator& operator=(const_iterator&&) noexcept = default;

        constexpr const_iterator(char const* first) noexcept :
            _first(first),
            _last(fixup_last(first)) {}

        [[nodiscard]] constexpr friend bool operator==(const_iterator const& lhs, const_iterator const& rhs) noexcept
        {
            return lhs._first == rhs._first;
        }

        [[nodiscard]] constexpr friend bool operator==(const_iterator const& lhs, std::default_sentinel_t) noexcept
        {
            return lhs._first == nullptr or *lhs._first == '\0';
        }

        constexpr const_iterator& operator++()
        {
            assert(_last != nullptr);

            _first = _last;
            if (*_first != '\0') {
                assert(*_first == '.');
                ++_first;
            }

            _last = fixup_last(_first);
            return *this;
        }

        constexpr const_iterator operator++(int)
        {
            auto tmp = *this;
            ++(*this);
            return tmp;
        }

        [[nodiscard]] constexpr std::string_view operator*() const
        {
            assert(_first != nullptr);
            assert(_last != nullptr);
            assert(_first <= _last);
            return std::string_view{_first, _last};
        }

        [[nodiscard]] constexpr pointer operator->() const
        {
            return pointer{**this};
        }

    private:
        char const* _first = nullptr;
        char const* _last = nullptr;

        [[nodiscard]] static char const* fixup_last(char const* p)
        {
            if (p != nullptr) {
                while (*p != '.' and *p != '\0') {
                    ++p;
                }
            }
            return p;
        }
    };

    ~fqname() = default;
    constexpr fqname() = default;
    constexpr fqname(fqname const&) = default;
    constexpr fqname(fqname&&) = default;
    constexpr fqname& operator=(fqname const&) = default;
    constexpr fqname& operator=(fqname&&) = default;

    constexpr explicit fqname(std::string_view other) : _str(other) {}
    constexpr explicit fqname(std::string other) : _str(std::move(other)) {}
    constexpr explicit fqname(char const *other) : _str(other) {}

    [[nodiscard]] constexpr friend bool operator==(fqname const& lhs, fqname const& rhs) noexcept
    {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    [[nodiscard]] constexpr friend bool operator==(fqname const& lhs, std::string_view rhs) noexcept
    {
        return lhs == fqname{rhs};
    }

    [[nodiscard]] constexpr friend std::strong_ordering operator<=>(fqname const& lhs, fqname const& rhs) noexcept
    {
        return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    [[nodiscard]] constexpr std::string const& string() const noexcept
    {
        return _str;
    }

    [[nodiscard]] constexpr size_t prefix() const noexcept
    {
        if (auto const i = _str.find_first_not_of('.'); i != _str.npos) {
            return i;
        } else {
            return _str.size();
        }
    }

    [[nodiscard]] constexpr bool is_absolute() const noexcept
    {
        return prefix() == 1;
    }

    [[nodiscard]] constexpr bool is_relative() const noexcept
    {
        return not is_absolute();
    }

    [[nodiscard]] constexpr std::string_view first_skip_prefix() const noexcept
    {
        auto const i = prefix();
        auto const j = _str.find('.', i);
        if (j == _str.npos) {
            return std::string_view{_str}.substr(i);
        } else {
            return std::string_view{_str}.substr(i, j - i);
        }
    }

    [[nodiscard]] constexpr std::string_view last() const noexcept
    {
        auto const i = _str.rfind('.');
        if (i == _str.npos) {
            return _str;
        } else {
            return std::string_view{_str}.substr(i + 1);
        }
    }

    [[nodiscard]] constexpr const_iterator begin() const noexcept
    {
        return const_iterator{_str.c_str()};
    }

    [[nodiscard]] constexpr const_iterator begin_skip_prefix() const noexcept
    {
        return const_iterator{_str.c_str() + prefix()};
    }

    [[nodiscard]] constexpr const_iterator end() const noexcept
    {
        return const_iterator{_str.c_str() + _str.size()};
    }

    /** Pop the last component.
     * 
     * Removes the last component, or:
     *  - if empty, the result is: ..
     *  - if absolute, the result is: .
     *  - if there are only dots, add one more dot.
     */
    constexpr fqname& pop_component()
    {
        using namespace std::literals;

        if (_str.empty()) {
            // Empty path is already relative, now go back by one.
            _str = ".."s;

        } else if (_str == "."s) {
            // The root, remains the root.

        } else if (prefix() == _str.size()) {
            // If there are just leading dots, add one more.
            _str += '.';

        } else if (auto const i = _str.rfind('.'); i == _str.npos) {
            // _str was a single component relative path, clear it completely.
            _str.clear();

        } else {
            // Remove only the component first.
            _str.erase(i + 1);

            if (prefix() != _str.size()) {
                // If there are more components remove the trailing dot.
                _str.erase(i);
            }
        }

        return *this;
    }

    constexpr fqname& add_component(std::string_view component)
    {
        using namespace std::literals;

        assert(component.find('.') == component.npos);

        if (component.empty()) {
            pop_component();

        } else {
            if (prefix() != _str.size()) {
                // If there was a component before, first add a trailing dot.
                _str += '.';
            }
            _str += component;
        }

        return *this;
    }

    /** Concatonate paths.
     * 
     *  - If rhs is an absolute path then it is returned.
     *  - Otherwise the paths are appended, maintaining the correct number of
     *    dots.
     */
    constexpr fqname& operator/=(fqname const& rhs)
    {
        if (rhs.prefix() == 1) {
            _str = rhs._str;
            return *this;

        } else if (rhs.prefix() == 0) {
            if (prefix() != _str.size()) {
                _str.reserve(_str.size() + 1 + rhs._str.size());
                _str += '.';
            }
            _str += rhs._str;

        } else {
            if (prefix() == _str.size()) {
                _str += std::string_view{rhs._str}.substr(1);   
            } else {
                _str += rhs._str;
            }
        }

        return *this;
    }

    constexpr fqname operator/(fqname const& rhs) const
    {
        auto tmp = *this;
        tmp /= rhs;
        return tmp;
    }

    constexpr fqname& operator/=(std::string_view rhs)
    {
        return *this /= fqname{rhs};
    }

    constexpr fqname operator/(std::string_view rhs) const
    {
        return *this / fqname{rhs};
    }

    /** Generate a lexically normal path.
     * 
     * Remove all double dot `..` from the path by removing the preceding name.
     * Unless at the prefix of the path where it will accumulate as an extra
     * dot.
     * 
     * Absolute paths will always remain absolute.
     * 
     * @return A path without any double dot `..`.
     */
    constexpr fqname lexically_normal() const
    {
        auto r = fqname{};
        r._str.reserve(_str.size());

        auto const pre = prefix();

        auto it = begin();
        for (auto i = 0uz; i != pre; ++i, ++it) {
            r._str += '.';
        }

        for (; it != end(); ++it) {
            r.add_component(*it);
        }

        return r;
    }

    /** Make a path relative to the base.
     * 
     *  1. If the this path is relative, append it to @a base.
     *  2. lexically_normal() the resulting path.
     */
    constexpr fqname lexically_absolute(fqname const& base)
    {
        return (base / *this).lexically_normal();
    }

    /** Make a path relative to the base.
     * 
     *  1. If the this path is relative, append it to @a base.
     *  2. lexically_normal() the resulting path.
     */
    constexpr fqname lexically_absolute(std::string_view base)
    {
        return lexically_absolute(fqname{base});
    }

private:
    std::string _str = {};

};

} // namespace hk


