
#pragma once

#include <cstdint>
#include <utility>
#include <exception>
#include <string_view>

namespace hk {

/** Logic type.
 * 
 * This is a logic-type a smaller version of VHDL's std_logic.
 * 
 */
enum class logic : uint8_t {
    /** False */
    F,
    /** True */
    T,
    /** Error */
    X,
    /** Don't Care (third state) */
    _,
};

/** Calculate a truth-table and pack it into a integer.
 * 
 * @param str A set of values represented by '0', '1', 'X', '-'. The left
 *            most character will places in the least significant 2 bits.
 */
[[nodiscard]] constexpr uint32_t make_logic_truth_table(std::string_view str)
{
    auto r = uint32_t{0};

    for (auto i = str.size(); i != 0; --i) {
        r <<= 2;
        switch (str[i - 1]) {
        case '0':
            r |= std::to_underlying(logic::F);
            break;
        case '1':
            r |= std::to_underlying(logic::T);
            break;
        case 'X':
            r |= std::to_underlying(logic::X);
            break;
        case '-':
            r |= std::to_underlying(logic::_);
            break;
        default:
            std::terminate();
        }
    }
    return r;
}

/** Create a truth table for logic functions.
 * 
 */
struct logic_truth_table {
    uint32_t _table;

    /** Create a truth table.
     * 
     * @param str A set of values represented by '0', '1', 'X', '-'. The left
     *            most character will places in the least significant 2 bits.
     */
    [[nodiscard]] constexpr logic_truth_table(std::string_view str) : _table(make_logic_truth_table(str)) {}

    /** Extract a value from the table.
     * 
     * @param lhs Index on the minor axis.
     * @param rhs Index on the major axis.
     * @return The value at the given coordinates
     */
    [[nodiscard]] constexpr logic operator[](logic lhs, logic rhs) const noexcept
    {
        auto tmp = _table;
        tmp >>= std::to_underlying(rhs) << 3;
        tmp >>= std::to_underlying(lhs) << 1;
        tmp &= 0b11;
        return static_cast<logic>(tmp);
    }

    /** Extract a value from the table.
     * 
     * @param lhs Index in the table.
     * @return The value at the given index
     */
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
    constexpr auto tt = logic_truth_table{"01X-11X1XXXX-1X-"};
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
    constexpr auto tt = logic_truth_table{"00X001X-XXXX0-X-"};
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
    constexpr auto tt = logic_truth_table{"01X-10X-XXXX--X-"};
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
    constexpr auto tt = logic_truth_table{"10X-"};
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
