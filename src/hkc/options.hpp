
#pragma once

#include "command.hpp"
#include <string>
#include <expected>
#include <filesystem>
#include <utility>
#include <vector>
#include <cstdint>
#include <string_view>
#include <map>
#include <set>

enum class command_type : uint8_t {
    none       = 0b000000,
    clone      = 0b000001,
    list       = 0b000011,
    build      = 0b000101,
    test       = 0b001001,
    sign       = 0b010101,
    distribute = 0b110101,
};

[[nodiscard]] constexpr command_type operator|(command_type lhs, command_type rhs)
{
    return static_cast<command_type>(std::to_underlying(lhs) | std::to_underlying(rhs));
}

[[nodiscard]] constexpr command_type operator&(command_type lhs, command_type rhs)
{
    return static_cast<command_type>(std::to_underlying(lhs) & std::to_underlying(rhs));
}

constexpr command_type &operator|=(command_type &lhs, command_type rhs)
{
    lhs = lhs | rhs;
    return lhs;
}

constexpr command_type &operator&=(command_type &lhs, command_type rhs)
{
    lhs = lhs & rhs;
    return lhs;
}

[[nodiscard]] constexpr bool to_bool(command_type cmd)
{
    return static_cast<bool>(std::to_underlying(cmd));
}


struct options {
    enum class build_type_t {
        debug,
        release,
    };

    enum class optimize_level_type {
        none,       // No optimizations
        basic,      // Basic optimizations
        aggressive, // Aggressive optimizations
        size,       // Size optimizations
    };

    /** The command(s) to for the compiler to run.
     */
    command_type command = command_type::none;

    /** The type of build to perform.
     */
    build_type_t build_type = build_type_t::debug;

    /** The optimization level to use during compilation.
     */
    optimize_level_type optimize_level = optimize_level_type::none;

    /** Show the help message.
     */
    bool help = false;
    
    /** Build and execute tests recursively in all cloned repositories. 
     */
    bool test_recursive = false;
    
    /** Build applications and libraries recursively in all cloned repositories.
     */
    bool build_recursive = false;

    /** Fetch the latest changes from remote repositories before building. 
     */
    bool fetch = false;

    /** A list of local repositories to include in the build.
     */
    std::set<std::filesystem::path> local_repositories = {};

    /** A list of library-paths to search for FFI libraries. 
     */
    std::set<std::filesystem::path> library_paths = {};

    /** Global variables to define during compilation. 
     */
    std::map<std::string, std::string> defines = {};

    /** The directory to compile. 
     */
    std::filesystem::path compile_directory = {};

    options();

    /** Parse command line options for the Hikolang compiler.
     * 
     * @param argc The number of command line arguments.
     * @param argv The command line arguments.
     * @return exit code.
     */
    [[nodiscard]] int parse(int argc, char const* const* argv);

private:
    [[nodiscard]] std::string finish();
};

