
#pragma once

#include <gsl/gsl>
#include <utility>
#include <print>
#include <cstdio>
#include <cassert>
#include <format>

namespace hk {

/** Enable ANSI codes on the terminal.
 * 
 * @note If unable to configure the terminal for ANSI, the log_mask::color
 *       flag is removed.
 */
void enable_ansi_codes();

enum class log_mask {
    /** Display progress for downloading.
     */
    progress,

    /** Use ANSI codes to display progress prettier.
     */
    color,

    /** Display debug information.
     */
    debug,

    /** Display verbose information when using resources outside the compiler.
     */
    verbose,

    /** Display warnings and errors for the compiler.
     *
     * Normally enabled, but disabled in case of a language server.
     */
    language,
};

[[nodiscard]] constexpr log_mask operator|(log_mask lhs, log_mask rhs) noexcept
{
    return static_cast<log_mask>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

[[nodiscard]] constexpr log_mask operator&(log_mask lhs, log_mask rhs) noexcept
{
    return static_cast<log_mask>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

[[nodiscard]] constexpr log_mask operator~(log_mask rhs) noexcept
{
    return static_cast<log_mask>(~std::to_underlying(rhs));
}

[[nodiscard]] constexpr bool to_bool(log_mask rhs) noexcept
{
    return static_cast<bool>(std::to_underlying(rhs));
}

inline log_mask global_log_mask = log_mask::progress | log_mask::color | log_mask::language;

template<typename... Args>
void log_lang(std::format_string<Args...> fmt, Args&&... args)
{
    if (not to_bool(global_log_mask & log_mask::language)) {
        return;
    }

    std::print(stderr, std::move(fmt), std::forward<Args>(args)...);
    std::fflush(stderr);
}

template<typename... Args>
void log_debug(std::format_string<Args...> fmt, Args&&... args)
{
    if (not to_bool(global_log_mask & log_mask::debug)) {
        return;
    }

    std::print(stderr, std::move(fmt), std::forward<Args>(args)...);
    std::fflush(stderr);
}

template<typename... Args>
void log_info(std::format_string<Args...> fmt, Args&&... args)
{
    if (not to_bool(global_log_mask & log_mask::verbose)) {
        return;
    }

    std::print(stderr, std::move(fmt), std::forward<Args>(args)...);
    std::fflush(stderr);
}

class progress {
public:
    progress(progress const&) = delete;
    progress(progress&&) = default;
    progress& operator=(progress const&) = delete;
    progress& operator=(progress&&) = default;

    ~progress()
    {
        if (not to_bool(global_log_mask & log_mask::progress)) {
            return;
        }

        std::print(stderr, "\n");
        std::fflush(stderr);
    }

    template<typename... Args>
    progress(std::format_string<Args...> fmt, Args&&... args) : _msg(std::format(std::move(fmt), std::forward<Args...>(args)...))
    {
        update();
    }

    progress& operator=(float value)
    {
        _value = value;
        update_display();
        return *this;
    }

    progress& operator=(std::string_view message)
    {
        assert(not message.empty());

        _msg = message;
        update_display();
        return *this;
    }

    void set_progress(float value)
    {
        assert(value >= _value);

        _value = value;
        update_display();
    }

    template<typename... Args>
    void set_message(std::format_string<Args...> fmt, Args&&... args)
    {
        _msg = std::format(std::move(fmt), std::forward<Args...>(args)...);
        update();
    }

private:
    std::string _msg = {};
    float _value = 0.0f;

    mutable std::string _old_msg = {};
    mutable float _old_value = 0.0f;

    /** Convert a value to ticks.
     * 
     * @param value Floating point value between 0.0 and 1.0
     * @param steps Number of steps between 0.0 and 1.0 (excluding 0.0)
     * @return The tick number.
     */
    [[nodiscard]] static unsigned int value_to_tick(float value, unsigned int num_ticks)
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
    void update_display_linear() const
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
    void update_display_color() const
    {
        enable_ansi_codes();

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

    void update_display() const
    {
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
};

} // namespace hk
