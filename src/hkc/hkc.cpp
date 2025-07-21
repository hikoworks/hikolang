
#include "options.hpp"
#include <utility/command_line.hpp>
#include <print>


int main(int argc, char const *const *argv)
{
    auto o = options{};
    if (auto exit_code = o.parse(argc, argv); exit_code != -1) {
        return exit_code;
    }

    return 0;
}