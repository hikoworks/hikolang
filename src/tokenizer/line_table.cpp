

#include "line_table.hpp"
#include "char_category.hpp"
#include <cassert>
#include <algorithm>
#include <format>

namespace hk {

[[nodiscard]] static std::pair<uint32_t, uint32_t> count_position(char const *first, char const *last) noexcept
{
    uint32_t line = 0;
    uint32_t column = 0;

    auto p = first;
    while (p < last) {
        auto const next = p + 1;

        if (match<char, '\n', '\v', '\f'>(p[0]) or (p[0] == '\r' and p[1] != '\n')) {
            ++line;
            column = 0;
            ++p;

        } else if (static_cast<int8_t>(p[0]) >= 0) {
            ++column;
            ++p;

        } else {
            auto const [cp, n] = get_cp(first);

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



[[nodiscard]] static char const* rfind_vertical_space(char const *first, char const *start) noexcept
{
    assert(start >= first);

    for (auto p = start - 1; p >= first; --p) {
        auto const c = static_cast<uint8_t>(p[0]);
        if ((c & 0xc0) == 0x80) {
            // Skip over UTF-8 continuation bytes.

        } else if (p < (start - 1) and p[0] == '\r' and p[1] != '\n') {
            // Found the CR/LF
            return p + 2;

        } else if (match<char, '\n', '\v', '\f'>(p[0])) {
            // Found a single character vertical space.
            return p + 1;

        } else if (c >= 0x80) {
            auto const [cp, n] = get_cp(p);

            if (match<char32_t, U'\u0085', U'\u2028', U'\u2029'>(cp)) {
                return p + n;
            }
        }
    }
    return first;
}

[[nodiscard]] static char const* find_vertical_space(char const* last, char const *start) noexcept
{
    assert(start <= last);

    for (auto p = start; p != last; ++p) {
        auto const c = static_cast<uint8_t>(p[0]);
        if ((p + 1) < last and p[0] == '\r' and p[1] == '\n') {
            // Found CR/LF
            return p;

        } else if (match<char, '\n', '\v', '\f'>(p[0])) {
            // Found a single character vertical space.
            return p;

        } else if (c >= 0xc0) {
            // Skip continuation bytes.

        } else if (c >= 0x80) {
            auto [cp, n] = get_cp(p);

            if (match<char32_t, U'\u0085', U'\u2028', U'\u2029'>(cp)) {
                return p;
            }
            p += n - 1;
        }
    }
    
    return last;
}

[[nodiscard]] static std::string_view get_line_text(char const *first, char const *last, char const *p)
{
    auto const first_on_line = rfind_vertical_space(first, p);
    auto const last_on_line = find_vertical_space(last, p);
    return std::string_view{first, last};
}

[[nodiscard]] std::tuple<interned_string, uint32_t, uint32_t, std::string_view> line_table::get_position(char const* p) const
{
    assert(p != nullptr);    
    assert(not _sync_points.empty());

    auto const it = std::lower_bound(_sync_points.begin(), _sync_points.end(), p, [](auto const& a, char const* x) {
        return a.p < x;
    });

    assert(it != _sync_points.end());
    assert(it->kind != sync_type::eof);

    auto const [extra_lines, column] = count_position(it->p, p);

    auto const first = it->p;
    auto const last = (it + 1)->p;
    auto const line_text = get_line_text(first, last, p);

    return {it->path, it->line + extra_lines, column, line_text};
}

[[nodiscard]] std::tuple<interned_string, uint32_t, uint32_t, std::string> line_table::get_error_location(
        char const *first, char const *last) const
{
    auto const [first_path, first_lineno, first_column, first_line] = get_position(first);

    auto r = std::format("{:5} | {}\n", first_lineno, first_line);

    r += "      | ";
    r += std::string(first_column, ' ');
    r += '^';
    if (last != nullptr) {
        auto const [last_path, last_lineno, last_column, last_line] = get_position(last);
        if (first_path == last_path and first_lineno == last_lineno) {
            assert(first_column <= last_column);

            r += std::string(last_column - first_column - 1, '~');
        }
    }
    r += '\n';

    return {first_path, first_lineno, first_column, r};
}

void line_table::add(char const* p, std::string_view path, uint32_t line, sync_type kind)
{
    if (_sync_points.empty()) {
        _sync_points.emplace_back(p, path, line, kind);
        return;
    }

    auto const it = std::lower_bound(_sync_points.begin(), _sync_points.end(), p, [](auto const &e, char const* x) {
        return e.p < x;
    });

    if (path.empty() and kind == sync_type::sol and it != _sync_points.begin()) {
        path = (it - 1)->path;
    }

    if (it->p == p) {
        // Duplicates are ignored, so that a file may be parsed twice.
        assert(it->path == path and it->line == line and it->kind == kind);
        return;
    }

    _sync_points.emplace(it, p, path, line, kind);    
}

void line_table::add_file(char const *begin, char const *end, std::string_view path)
{
    add(begin, path, 0, sync_type::sof);
    add(end, path, 0, sync_type::eof);
}

void line_table::add_sol(char const* p, uint32_t line)
{
    using namespace std::literals;

    return add(p, ""sv, line, sync_type::sol);
}

void line_table::add_sol(char const* p, std::string_view path, uint32_t line)
{
    return add(p, path, line, sync_type::sol);
}


} // namespace hk
