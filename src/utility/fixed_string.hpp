
#pragma once

#include <cstddef>
#include <string_view>

namespace hl {

/** A fixed imutable string type that can be used as a template parameter. 
 * 
 * @tparam N The size of the string, excluding the null terminator.
 * @tparam CharT The character type of the string, defaults to char.
 */
template<std::size_t N>
struct fixed_string {
    using size_type = std::size_t;
    static constexpr size_type max_size = N;

    char _data[N];

    constexpr fixed_string(fixed_string const&) noexcept = default;
    [[nodiscard]] constexpr friend bool operator==(fixed_string const&, fixed_string const&) noexcept = default;
    
    constexpr fixed_string(char const (&str)[N + 1]) noexcept
    {
        for (auto i = 0uz; i != N; ++i) {
            _data[i] = str[i];
        }
    }

    constexpr operator std::string_view() const noexcept
    {
        return std::string_view(_data, N);
    }

    [[nodiscard]] constexpr char const* data() const noexcept
    {
        return _data;
    }

    [[nodiscard]] constexpr char operator[](size_type i) const noexcept
    {
        return _data[i];
    }

    [[nodiscard]] constexpr size_type size() const noexcept
    {
        return N;
    }

};

template<std::size_t M>
fixed_string(char const (&str)[M]) -> fixed_string<M - 1>;
    
}