

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
    _file_names.clear();
}

std::tuple<std::string, size_t, size_t> line_table::get_position(char const* p) const
{
    if (p == nullptr) {
        return {{}, 0uz, 0uz};
    }
    
    assert(not _sync_points.empty());

    auto it = std::lower_bound(_sync_points.begin(), _sync_points.end(), p, [](sync_point const& a, char const* x) {
        return a.p < x;
    });

    if (it != _sync_points.end() and it->p == p) {
        return {_file_names.at(it->fileno), it->fileno, 0};
    }

    // The correct entry is one before the upper-bound. This entry should have
    // a pointer that is less than or equal to the searched pointer.
    assert(it != _sync_points.begin());
    --it;
    assert(it->p < p);

    auto [extra_lines, column] = count_position(it->p, p);
    return {_file_names.at(it->fileno), it->fileno + extra_lines, column};
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

void line_table::add(char const* p, size_t lineno)
{
    assert(not _sync_points.empty());

    if (p > _sync_points.back().p) {
        // Fast append.
        _sync_points.emplace_back(p, lineno, _sync_points.back().fileno);
        return;
    }

    auto it = std::lower_bound(_sync_points.begin(), _sync_points.end(), p, [](sync_point const &e, char const* x) {
        return e.p < x;
    });

    if (it != _sync_points.end() and it->p == p) {
        // Entry already exists.
        assert(it->lineno == lineno);
        return;
    }

    // Insert the new entry.
    assert(it != _sync_points.begin());
    _sync_points.emplace(it, p, lineno, (it - 1)->fileno);
}

void line_table::add(char const* p, size_t lineno, std::string_view file_name)
{
    auto const fileno = get_fileno(file_name);

    if (_sync_points.empty() or p > _sync_points.back().p) {
        // Fast append.
        _sync_points.emplace_back(p, lineno, fileno);
        return;
    }

    auto it = std::lower_bound(_sync_points.begin(), _sync_points.end(), p, [](sync_point const &e, char const* x) {
        return e.p < x;
    });

    if (it != _sync_points.end() and it->p == p) {
        // Entry already exists.
        assert(it->lineno == lineno);
        assert(it->fileno == fileno);
        return;
    }

    // Insert the new entry.
    _sync_points.emplace(it, p, lineno, fileno);
}

[[nodiscard]] size_t line_table::get_fileno(std::string_view file_name)
{
    for (auto i = 0uz; i != _file_names.size(); ++i) {
        if (file_name == _file_names[i]) {
            return i;
        }
    }
    _file_names.emplace_back(file_name);
    return _file_names.size() - 1;
}

} // namespace hk
