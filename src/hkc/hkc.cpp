
#include "options.hpp"
#include <utility/command_line.hpp>
#include <print>


int main(int argc, char const *const *argv)
{
    auto options = parse_options(argc, argv);
    if (not options) {
        return options.error();
    }

    return 0;
}