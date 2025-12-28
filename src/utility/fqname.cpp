
#include "fqname.hpp"
#include <algorithm>

namespace hk {

fqname::const_iterator::const_iterator(char const* first) noexcept : _first(first), _last(fixup_last(first)) {}

[[nodiscard]] bool operator==(fqname::const_iterator const& lhs, fqname::const_iterator const& rhs) noexcept
{
    return lhs._first == rhs._first;
}

[[nodiscard]] bool operator==(fqname::const_iterator const& lhs, std::default_sentinel_t) noexcept
{
    return lhs._first == nullptr or *lhs._first == '\0';
}

fqname::const_iterator& fqname::const_iterator::operator++()
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

fqname::const_iterator fqname::const_iterator::operator++(int)
{
    auto tmp = *this;
    ++(*this);
    return tmp;
}

[[nodiscard]] std::string_view fqname::const_iterator::operator*() const
{
    assert(_first != nullptr);
    assert(_last != nullptr);
    assert(_first <= _last);
    return std::string_view{_first, _last};
}

[[nodiscard]] fqname::const_iterator::pointer fqname::const_iterator::operator->() const
{
    return pointer{**this};
}

[[nodiscard]] char const* fqname::const_iterator::fixup_last(char const* p)
{
    if (p != nullptr) {
        while (*p != '.' and *p != '\0') {
            ++p;
        }
    }
    return p;
}

[[nodiscard]] bool operator==(fqname const& lhs, fqname const& rhs) noexcept
{
    return std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

[[nodiscard]] bool operator==(fqname const& lhs, std::string_view rhs) noexcept
{
    return lhs == fqname{rhs};
}

[[nodiscard]] std::strong_ordering operator<=>(fqname const& lhs, fqname const& rhs) noexcept
{
    return std::lexicographical_compare_three_way(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

[[nodiscard]] size_t fqname::prefix() const noexcept
{
    if (auto const i = _str.find_first_not_of('.'); i != _str.npos) {
        return i;
    } else {
        return _str.size();
    }
}

[[nodiscard]] bool fqname::is_absolute() const noexcept
{
    return prefix() == 1;
}

[[nodiscard]] bool fqname::is_relative() const noexcept
{
    return not is_absolute();
}


[[nodiscard]] std::string_view fqname::first_skip_prefix() const noexcept
{
    auto const i = prefix();
    auto const j = _str.find('.', i);
    if (j == _str.npos) {
        return std::string_view{_str}.substr(i);
    } else {
        return std::string_view{_str}.substr(i, j - i);
    }
}

[[nodiscard]] std::string_view fqname::last() const noexcept
{
    auto const i = _str.rfind('.');
    if (i == _str.npos) {
        return _str;
    } else {
        return std::string_view{_str}.substr(i + 1);
    }
}

[[nodiscard]] fqname::const_iterator fqname::begin() const noexcept
{
    return const_iterator{_str.c_str()};
}

[[nodiscard]] fqname::const_iterator fqname::begin_skip_prefix() const noexcept
{
    return const_iterator{_str.c_str() + prefix()};
}

[[nodiscard]] fqname::const_iterator fqname::end() const noexcept
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
fqname& fqname::pop_component()
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

fqname& fqname::add_component(std::string_view component)
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
fqname& fqname::operator/=(fqname const& rhs)
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

fqname fqname::operator/(fqname const& rhs) const
{
    auto tmp = *this;
    tmp /= rhs;
    return tmp;
}

fqname& fqname::operator/=(std::string_view rhs)
{
    return *this /= fqname{rhs};
}

fqname fqname::operator/(std::string_view rhs) const
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
fqname fqname::lexically_normal() const
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
fqname fqname::lexically_absolute(fqname const& base)
{
    return (base / *this).lexically_normal();
}

/** Make a path relative to the base.
 *
 *  1. If the this path is relative, append it to @a base.
 *  2. lexically_normal() the resulting path.
 */
fqname fqname::lexically_absolute(std::string_view base)
{
    return lexically_absolute(fqname{base});
}

[[nodiscard]] bool is_child_of(fqname const& child, fqname const& parent) noexcept
{
    auto jt = child.begin();
    for (auto it = parent.begin(); it != parent.end(); ++it, ++jt) {
        if (jt == child.end() or *it != *jt) {
            return false;
        }
    }
    return true;
}

} // namespace hk