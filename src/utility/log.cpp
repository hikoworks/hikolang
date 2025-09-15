
#include "log.hpp"
#if defined(_WIN32)
#include <Windows.h>
#endif
#include <gsl/gsl>
#include <cmath>
#include <print>
#include <cstdio>
#include <string>

namespace hk {

class enable_ansi_codes_helper {
public:
    ~enable_ansi_codes_helper() = default;

    constexpr enable_ansi_codes_helper() noexcept = default;

    enable_ansi_codes_helper(bool enable)
    {
        if (not enable) {
            _enabled = false;
        }

#if defined(_WIN32)
        if (not enable_ansi_codes(STD_OUTPUT_HANDLE)) {
            global_log_mask = global_log_mask & ~log_mask::color;
            return;
        }

        if (not enable_ansi_codes(STD_ERROR_HANDLE)) {
            global_log_mask = global_log_mask & ~log_mask::color;
            return;
        }
#endif
        _enabled = true;
    }

    [[nodiscard]] bool enabled() const noexcept
    {
        return _enabled;
    }

private:
    bool _enabled = false;

#if defined(_WIN32)
    [[nodiscard]] static bool enable_ansi_codes(DWORD device)
    {
        auto const handle = GetStdHandle(device);
        if (handle == INVALID_HANDLE_VALUE) {
            return false;
        }

        DWORD mode = 0;
        if (GetConsoleMode(handle, &mode) == 0) {
            return false;
        }
        if (SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) == 0) {
            return false;
        }
        return true;
    }
#endif
};

[[nodiscard]] bool enable_ansi_codes()
{
    static auto init = enable_ansi_codes_helper{};
    return init.enabled();
}

/** Convert a value to ticks.
 *
 * @param value Floating point value between 0.0 and 1.0
 * @param steps Number of steps between 0.0 and 1.0 (excluding 0.0)
 * @return The tick number.
 */
[[nodiscard]] unsigned int progress::value_to_tick(float value, unsigned int num_ticks)
{
    value = std::clamp(value, 0.0f, 1.0f);
    value *= gsl::narrow<float>(num_ticks);

    // Round the value to the next tick, only when it is 90% on its way
    // to that tick.
    value *= 10.0f;
    value = std::round(value);
    value /= 10.0f;
    value = std::floor(value);

    return gsl::narrow<unsigned int>(value);
}

/** Update the display on a non-ANSI capable terminal
 *
 * It shows the message, followed by percentages, increasing by 20%.
 * For example:
 *    `My progress message. 20% 40% 60%`
 *
 * This version can not handle:
 *  - When the message changes, only the first message is displayed.
 */
void progress::update_display_linear() const
{
    auto flush = false;

    if (_old_msg.empty()) {
        std::print(stderr, "{}", _msg);
        flush = true;
    }

    // Ticks every 10%.
    auto const ticks = value_to_tick(_value, 5);
    auto const old_ticks = value_to_tick(_value, 5);
    if (old_ticks < ticks) {
        std::print(stderr, " {}%", ticks * 20);
        flush = true;
    }

    if (flush) {
        std::fflush(stderr);
    }
}

/** Update the display on a ANSI capable terminal.
 *
 * Display looks as follows:
 *  `[====      ] My progress message.`
 */
void progress::update_display_color() const
{
    auto flush = false;
    auto first_time = _old_msg.empty();

    auto const ticks = value_to_tick(_value, 10);
    auto const old_ticks = value_to_tick(_value, 10);
    if (old_ticks < ticks or first_time) {
        assert(ticks <= 10);

        auto bar = std::string{};
        auto i = 0u;
        for (; i != ticks; ++i) {
            bar += '=';
        }
        for (; i != 10; ++i) {
            bar += " ";
        }

        // ANSI: go to column 1.
        std::print(stderr, "\033[G [{}]", bar);
        flush = true;
    }

    if (_old_msg != _msg) {
        // ANSI: go to column 15. clear to end of line.
        std::print(stderr, "\033[15G\033[K{}", _msg);
        flush = true;
    }

    if (flush) {
        std::fflush(stderr);
    }
}

void progress::update_display() const
{
    if (to_bool(global_log_mask & log_mask::progress) and to_bool(global_log_mask & log_mask::color)) {
        if (not enable_ansi_codes()) {
            global_log_mask &= ~log_mask::color;
        }
    }

    if (to_bool(global_log_mask & log_mask::progress)) {
        if (to_bool(global_log_mask & log_mask::color)) {
            update_display_color();
        } else {
            update_display_linear();
        }
    }

    _old_msg = _msg;
    _old_value = _value;
}

} // namespace hk
