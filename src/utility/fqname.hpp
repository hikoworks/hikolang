
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
            _last(fixup_last(first))

                [[nodiscard]] constexpr bool operator==(const_iterator const& rhs) noexcept
        {
            return _first == rhs._first;
        }

        [[nodiscard]] constexpr bool operator==(std::default_sentinel_t) noexcept
        {
            return _first == nullptr or *_first == '\0';
        }

        [[nodiscard]] constexpr std::strong_ordering operator<=>(const_iterator const& rhs) noexcept
        {
            return _first <=> rhs._first;
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

    [[nodiscard]] constexpr std::string const& string() const noexcept
    {
        return _str;
    }

    [[nodiscard]] const_iterator begin() const noexcept
    {
        return const_iterator{_str.c_str()};
    }

    [[nodiscard]] const_iterator end() const noexcept
    {
        return const_iterator{_str.c_str() + _str.size()};
    }

    bool pop_component()
    {
        if (_str.empty()) {
            return false;
        }

        if (auto const i = _str.rfind('.'); i == _str.npos) {
            _str.clear();
            return true;
        } else {
            _str.erase(i);
            return true;
        }
    }

    fqname& operator/=(std::string_view component)
    {
        assert(component.empty() or (component.first() != '.' and component.last() != '.'));

        if (not _str.empty()) {
            _str += '.';
        }
        _str += component;
        return *this;
    }

    [[nodiscard]] fqname operator/(std::string_view component)
    {
        auto tmp = *this;
        tmp /= component;
        return tmp;
    }

    fqname& operator/=(fqname const& other)
    {
        if (other.is_absolute()) {
            *this = other;
            return *this;
        }

        auto leading = true;
        for (auto component : other) {
            if (component.empty()) {
                assert(leading);
                pop_component();
            } else {
                leading = false;
                *this /= component;
            }
        }
    }

private:
    std::string _str = {};
};

} // namespace hk


