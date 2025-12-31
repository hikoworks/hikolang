
#pragma once

#include <cstdint>
#include <utility>
#include <stdexcept>

namespace hk {

enum class logic : uint8_t {
    F,
    T,
    X,
    _,
};

template<size_t N>
[[nodiscard]] constexpr auto make_logic_truth_table(char const *str)
{
    using int_type = std::conditional_t<N == 16, uint32_t, uint8_t>;
    auto r = int_type{};

    for (auto i = N; i != 0; --i) {
        r <<= 2;
        switch (str[i - 1]) {
        case 'F':
        case '0':
            r |= std::to_underlying(logic::F);
            break;
        case 'T':
        case '1':
            r |= std::to_underlying(logic::T);
            break;
        case 'X':
            r |= std::to_underlying(logic::X);
            break;
        case '_':
        case '-':
            r |= std::to_underlying(logic::_);
            break;
        default:
            std::terminate();
        }
    }
    return r;
}

struct logic_binary_truth_table {
    uint32_t _table;

    [[nodiscard]] constexpr logic_binary_truth_table(char const str[17]) : _table(make_logic_truth_table<16>(str)) {}

    [[nodiscard]] constexpr logic operator[](logic lhs, logic rhs) const noexcept
    {
        auto tmp = _table;
        tmp >>= std::to_underlying(rhs) << 3;
        tmp >>= std::to_underlying(lhs) << 1;
        tmp &= 0b11;
        return static_cast<logic>(tmp);
    }
};

struct logic_unary_truth_table {
    uint8_t _table;

    [[nodiscard]] constexpr logic_unary_truth_table(char const str[5]) : _table(make_logic_truth_table<4>(str)) {}

    [[nodiscard]] constexpr logic operator[](logic rhs) const noexcept
    {
        auto tmp = _table;
        tmp >>= std::to_underlying(rhs) << 1;
        tmp &= 0b11;
        return static_cast<logic>(tmp);
    }
};

/** to_bool
 *
 *  | 0 | 1 | X | - |
 *  |---|---|---|---|
 *  | 0 | 1 | 0 | 1 |
 */
[[nodiscard]] constexpr bool to_bool(logic rhs) noexcept
{
    return static_cast<bool>(std::to_underlying(rhs) & 0b01);
}

/** to_logic
 *
 *  | 0 | 1 |
 *  |---|---|
 *  | 0 | 1 |
 */
[[nodiscard]] constexpr logic to_logic(bool const& rhs) noexcept
{
    return static_cast<logic>(rhs);
}

/** logical OR
 *
 *  |   || 0 | 1 | x | - |
 *  |---||---|---|---|---|
 *  | 0 || 0 | 1 | X | - |
 *  | 1 || 1 | 1 | X | 1 |
 *  | X || X | X | X | X |
 *  | - || - | 1 | X | - |
 */
[[nodiscard]] constexpr logic operator|(logic lhs, logic rhs) noexcept
{
    constexpr auto tt = logic_binary_truth_table{"01X-11X1XXXX-1X-"};
    return tt[lhs, rhs];
}

/** logical AND
 *
 *  |   || 0 | 1 | X | - |
 *  |---||---|---|---|---|
 *  | 0 || 0 | 0 | X | 0 |
 *  | 1 || 0 | 1 | X | - |
 *  | X || X | X | X | X |
 *  | - || 0 | - | X | - |
 */
[[nodiscard]] constexpr logic operator&(logic lhs, logic rhs) noexcept
{
    constexpr auto tt = logic_binary_truth_table{"00X001X-XXXX0-X-"};
    return tt[lhs, rhs];
}

/** logical XOR
 *
 *  |   || 0 | 1 | X | X |
 *  |---||---|---|---|---|
 *  | 0 || 0 | 1 | X | - |
 *  | 1 || 1 | 0 | X | - |
 *  | X || X | X | X | X |
 *  | - || - | - | X | - |
 */
[[nodiscard]] constexpr logic operator^(logic lhs, logic rhs) noexcept
{
    constexpr auto tt = logic_binary_truth_table{"01X-10X-XXXX--X-"};
    return tt[lhs, rhs];
}

/** logical Invert
 *
 *  | 0 | 1 | X | - |
 *  |---|---|---|---|
 *  | 1 | 0 | X | - |
 */
[[nodiscard]] constexpr logic operator~(logic rhs) noexcept
{
    constexpr auto tt = logic_unary_truth_table{"10X-"};
    return tt[rhs];
}

constexpr logic& operator|=(logic& lhs, logic rhs) noexcept
{
    return lhs = lhs | rhs;
}

constexpr logic& operator&=(logic& lhs, logic rhs) noexcept
{
    return lhs = lhs & rhs;
}

constexpr logic& operator^=(logic& lhs, logic rhs) noexcept
{
    return lhs = lhs ^ rhs;
}

[[nodiscard]] constexpr logic operator|(logic lhs, bool rhs) noexcept
{
    return lhs | to_logic(rhs);
}

[[nodiscard]] constexpr logic operator&(logic lhs, bool rhs) noexcept
{
    return lhs & to_logic(rhs);
}

[[nodiscard]] constexpr logic operator^(logic lhs, bool rhs) noexcept
{
    return lhs ^ to_logic(rhs);
}

[[nodiscard]] constexpr logic operator|(bool lhs, logic rhs) noexcept
{
    return to_logic(lhs) | rhs;
}

[[nodiscard]] constexpr logic operator&(bool lhs, logic rhs) noexcept
{
    return to_logic(lhs) & rhs;
}

[[nodiscard]] constexpr logic operator^(bool lhs, logic rhs) noexcept
{
    return to_logic(lhs) ^ rhs;
}

constexpr logic& operator|=(logic& lhs, bool rhs) noexcept
{
    return lhs = lhs | to_logic(rhs);
}

constexpr logic& operator&=(logic& lhs, bool rhs) noexcept
{
    return lhs = lhs & to_logic(rhs);
}

constexpr logic& operator^=(logic& lhs, bool rhs) noexcept
{
    return lhs = lhs ^ to_logic(rhs);
}

[[nodiscard]] constexpr bool operator!(logic rhs) noexcept
{
    return not to_bool(rhs);
}

[[nodiscard]] constexpr bool operator||(logic lhs, logic rhs) noexcept
{
    return to_bool(lhs | rhs);
}

[[nodiscard]] constexpr bool operator&&(logic lhs, logic rhs) noexcept
{
    return to_bool(lhs & rhs);
}

[[nodiscard]] constexpr bool operator||(logic lhs, bool rhs) noexcept
{
    return to_bool(lhs | rhs);
}

[[nodiscard]] constexpr bool operator&&(logic lhs, bool rhs) noexcept
{
    return to_bool(lhs & rhs);
}

[[nodiscard]] constexpr bool operator||(bool lhs, logic rhs) noexcept
{
    return to_bool(lhs | rhs);
}

[[nodiscard]] constexpr bool operator&&(bool lhs, logic rhs) noexcept
{
    return to_bool(lhs & rhs);
}

} // namespace hk
