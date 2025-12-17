
#pragma once

#include <gsl/gsl>
#include <string>
#include <cassert>
#include <iterator>
#include <cstddef>
#include <string_view>
#include <memory>
#include <bit>
#include <compare>
#include <format>

namespace hk {

/** A fully qualified name.
 *
 *
 */
class fqname {
public:
    class const_iterator {
    public:
        using difference_type = std::ptrdiff_t;
        using value_type = std::string_view;
        using reference = std::string_view;
        using iterator_category = std::input_iterator_tag;

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

        const_iterator(char const* first) noexcept;

        friend bool operator==(const_iterator const& lhs, const_iterator const& rhs) noexcept;

        friend bool operator==(const_iterator const& lhs, std::default_sentinel_t) noexcept;

        const_iterator& operator++();

        const_iterator operator++(int);

        [[nodiscard]] std::string_view operator*() const;

        [[nodiscard]] pointer operator->() const;

    private:
        char const* _first = nullptr;
        char const* _last = nullptr;

        [[nodiscard]] static char const* fixup_last(char const* p);
    };

    static_assert(std::input_iterator<const_iterator>);

    ~fqname() = default;
    constexpr fqname() = default;
    constexpr fqname(fqname const&) = default;
    constexpr fqname(fqname&&) = default;
    constexpr fqname& operator=(fqname const&) = default;
    constexpr fqname& operator=(fqname&&) = default;

    constexpr explicit fqname(std::string_view other) : _str(other) {}
    constexpr explicit fqname(std::string other) : _str(std::move(other)) {}
    constexpr explicit fqname(char const *other) : _str(other) {}

    friend bool operator==(fqname const& lhs, fqname const& rhs) noexcept;

    friend bool operator==(fqname const& lhs, std::string_view rhs) noexcept;

    friend std::strong_ordering operator<=>(fqname const& lhs, fqname const& rhs) noexcept;

    [[nodiscard]] constexpr std::string const& string() const noexcept
    {
        return _str;
    }

    [[nodiscard]] size_t prefix() const noexcept;
    [[nodiscard]] bool is_absolute() const noexcept;

    [[nodiscard]] bool is_relative() const noexcept;

    [[nodiscard]] std::string_view first_skip_prefix() const noexcept;

    [[nodiscard]] std::string_view last() const noexcept;

    [[nodiscard]] const_iterator begin() const noexcept;

    [[nodiscard]] const_iterator begin_skip_prefix() const noexcept;

    [[nodiscard]] const_iterator end() const noexcept;

    /** Pop the last component.
     * 
     * Removes the last component, or:
     *  - if empty, the result is: ..
     *  - if absolute, the result is: .
     *  - if there are only dots, add one more dot.
     */
    fqname& pop_component();

    fqname& add_component(std::string_view component);

    /** Concatonate paths.
     * 
     *  - If rhs is an absolute path then it is returned.
     *  - Otherwise the paths are appended, maintaining the correct number of
     *    dots.
     */
    fqname& operator/=(fqname const& rhs);

    fqname operator/(fqname const& rhs) const;

    fqname& operator/=(std::string_view rhs);

    fqname operator/(std::string_view rhs) const;

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
    fqname lexically_normal() const;

    /** Make a path relative to the base.
     * 
     *  1. If the this path is relative, append it to @a base.
     *  2. lexically_normal() the resulting path.
     */
    fqname lexically_absolute(fqname const& base);

    /** Make a path relative to the base.
     * 
     *  1. If the this path is relative, append it to @a base.
     *  2. lexically_normal() the resulting path.
     */
    fqname lexically_absolute(std::string_view base);

private:
    std::string _str = {};

};

[[nodiscard]] bool operator==(fqname::const_iterator const& lhs, fqname::const_iterator const& rhs) noexcept;

[[nodiscard]] bool operator==(fqname::const_iterator const& lhs, std::default_sentinel_t) noexcept;

[[nodiscard]] bool operator==(fqname const& lhs, fqname const& rhs) noexcept;

[[nodiscard]] bool operator==(fqname const& lhs, std::string_view rhs) noexcept;

[[nodiscard]] std::strong_ordering operator<=>(fqname const& lhs, fqname const& rhs) noexcept;

} // namespace hk

template<>
struct std::formatter<hk::fqname, char> : public std::formatter<std::string, char> {
    template<class FmtContext>
    FmtContext::iterator format(hk::fqname x, FmtContext& ctx) const
    {
        return std::formatter<std::string, char>::format(x.string(), ctx);
    }
};



