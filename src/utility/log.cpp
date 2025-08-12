
#include "log.hpp"
#if defined(_WIN32)
#include <Windows.h>
#endif

namespace hk {

class enable_ansi_codes_helper {
public:
    ~enable_ansi_codes_helper() = default;

    

    enable_ansi_codes_helper()
    {
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
    }

private:
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

void enable_ansi_codes()
{
    static auto init = enable_ansi_codes_helper{};
}

}
