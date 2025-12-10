
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
            pointer(std::string_view other) noexcept : _value(other) {}

            [[nodiscard]] std::string_view const* operator->() const noexcept
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

    constexpr fqname(std::string_view other) : _str(other) {}
    constexpr fqname(std::string other) : _str(std::move(other)) {}
    constexpr fqname(char const *other) : _str(other) {}

    [[nodiscard]] constexpr friend bool operator==(fqname const& lhs, fqname const& rhs) noexcept
    {
        return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
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

    [[nodiscard]] constexpr const_iterator begin() const noexcept
    {
        return const_iterator{_str.c_str()};
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
    constexpr void pop_component()
    {
        using namespace std::literals;

        if (_str.empty()) {
            _str = ".."s;
        } else if (auto const p = prefix(); p == _str.size()) {
            if (p != 1) {
                _str += '.';
            }
        } else {
            if (auto const i = _str.rfind('.'); i == _str.npos) {
                _str.clear();
            } else {
                _str.erase(i);
            }
        }
    }

    constexpr fqname& add_component(std::string_view component)
    {
        assert(component.find('.') == component.npos);

        if (component.empty()) {
            _str += '.';

        } else {
            if (not _str.empty()) {
                _str += '.';
            }
            _str += component;
        }
        return *this;
    }

    constexpr fqname& operator/=(fqname const& rhs)
    {
        if (rhs.is_absolute()) {
            *this = rhs;
            return *this;
        }

        for (auto component : rhs) {
            if (component.empty()) {
                pop_component();
            } else {
                add_component(component);
            }
        }
        return *this;
    }

    constexpr fqname operator/(fqname const& rhs)
    {
        auto tmp = *this;
        tmp /= rhs;
        return tmp;
    }

    constexpr fqname& operator/=(std::string_view rhs)
    {
        return *this /= fqname{rhs};
    }

    constexpr fqname operator/(std::string_view rhs)
    {
        return *this / fqname{rhs};
    }

    /** Generate a lexically normal path.
     * 
     */
    constexpr fqname lexically_normal() const
    {
        auto r = fqname{};
        r._str.reserve(_str.size());

        auto is_prefix = true;
        for (auto component: *this) {
            if (component.empty()) {
                if (is_prefix) {
                    r.add_component(component);
                } else {
                    r.pop_component();
                }
            } else {
                r.add_component(component);
                is_prefix = false;
            }
        }

        return r;
    }

private:
    std::string _str = {};

};

} // namespace hk


