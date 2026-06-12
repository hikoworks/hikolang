

#include "line_table.hpp"
#include "char_category.hpp"
#include <cassert>
#include <algorithm>

namespace hk {

[[nodiscard]] static std::pair<size_t, size_t> count_position(char const *first, char const *last) noexcept
{
    auto line = 0;
    auto column = 0;

    auto p = first;
    while (p < last) {
        auto next = p + 1;

        if (match<char, '\n', '\v', '\f'>(p[0]) or (p[0] == '\r' and p[1] != '\n')) {
            ++line;
            column = 0;
            ++p;

        } else if (static_cast<int8_t>(p[0]) >= 0) {
            ++column;
            ++p;

        } else {
            auto [cp, n] = get_cp(first);

            if (match<char32_t, U'\u0085', U'\u2028', U'\u2029'>(cp)) {
                ++line;
                column = 0;

            } if (cp >= 0x01'0000) {
                // code-point is encoded as a surrogate pair in UTF-16.
                column += 2;

            } else {
                ++column;
            }

            p += n;
            assert(p <= last);
        }
    }

    return {line, column};
}

void line_table::clear()
{
    _sync_points.clear();
}

std::tuple<interned_string, size_t, size_t> line_table::get_position(char const* p) const
{
    if (p == nullptr) {
        return {{}, 0uz, 0uz};
    }
    
    assert(not _sync_points.empty());

    auto it = std::lower_bound(_sync_points.begin(), _sync_points.end(), p, [](sync_point const& a, char const* x) {
        return a.p < x;
    });

    if (it != _sync_points.end() and it->p == p) {
        return {it->path, it->line, 0};
    }

    // The correct entry is one before the upper-bound. This entry should have
    // a pointer that is less than or equal to the searched pointer.
    assert(it != _sync_points.begin());
    --it;
    assert(it->p < p);

    auto [extra_lines, column] = count_position(it->p, p);
    return {it->path, it->line + extra_lines, column};
}

[[nodiscard]] static char const* rfind_vertical_space(char const *p, char const *begin) noexcept
{
    assert(p >= begin);
    for (--p; p >= begin; --p) {
        auto const c = static_cast<uint8_t>(p[0]);
        if ((c & 0xc0) == 0x80) {
            // Skip over UTF-8 continuation bytes.
        } else if (match<char, '\n', '\v', '\f'>(p[0]) or (p[0] == '\r' and p[1] != '\n')) {
            // Found a single character vertical space.
            return p + 1;
        } else if (c >= 0x80) {
            auto [cp, n] = get_cp(p);

            if (match<char32_t, U'\u0085', U'\u2028', U'\u2029'>(cp)) {
                return p + n;
            }
        }
    }
    return begin;
}

[[nodiscard]] static char const* find_vertical_space(char const *p) noexcept
{
    for (; p[0] != '\0'; ++p) {
        auto const c = static_cast<uint8_t>(p[0]);
        if (match<char, '\n', '\v', '\f'>(p[0]) or (p[0] == '\r' and p[1] != '\n')) {
            // Found a single character vertical space.
            return p;

        } else if (c >= 0x80) {
            auto [cp, n] = get_cp(p);

            if (match<char32_t, U'\u0085', U'\u2028', U'\u2029'>(cp)) {
                return p;
            }
            p += n - 1;
        }
    }
    return p;
}

[[nodiscard]] std::string_view line_table::get_line_text(char const *p) const
{
    assert(not _sync_points.empty());
    auto begin = _sync_points.front().p;

    auto first = rfind_vertical_space(p, begin);
    auto last = find_vertical_space(p);
    return std::string_view{first, last};
}

void line_table::add(char const* p, size_t line)
{
    assert(not _sync_points.empty());

    if (p > _sync_points.back().p) {
        // Fast append.
        _sync_points.emplace_back(p, line, _sync_points.back().path);
        return;
    }

    auto it = std::lower_bound(_sync_points.begin(), _sync_points.end(), p, [](sync_point const &e, char const* x) {
        return e.p < x;
    });

    if (it != _sync_points.end() and it->p == p) {
        // Entry already exists.
        assert(it->line == line);
        return;
    }

    // Insert the new entry.
    assert(it != _sync_points.begin());
    _sync_points.emplace(it, p, line, (it - 1)->path);
}

void line_table::add(char const* p, size_t line, std::string_view path)
{
    if (_sync_points.empty() or p > _sync_points.back().p) {
        // Fast append.
        _sync_points.emplace_back(p, line, path);
        return;
    }

    auto it = std::lower_bound(_sync_points.begin(), _sync_points.end(), p, [](sync_point const &e, char const* x) {
        return e.p < x;
    });

    if (it != _sync_points.end() and it->p == p) {
        // Entry already exists.
        assert(it->line == line);
        assert(it->path == path);
        return;
    }

    // Insert the new entry.
    _sync_points.emplace(it, p, line, path);
}

} // namespace hk
