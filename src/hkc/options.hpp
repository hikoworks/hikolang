
#pragma once

#include "command.hpp"
#include <string>
#include <expected>
#include <filesystem>



struct options {
    /** The command to execute.
     */
    command cmd = command::none;

    /** The command to request help for.
     */
    command cmd_help = command::none;

    /** Download packages from the internet, to make sure that the latest version is used. 
     */
    bool force_download_packages = false; 

    /** The directory to compile. 
     * 
     */
    std::filesystem::path compile_directory = {};

};

/** Parse command line options for the Hikolang compiler.
 * 
 * @param argc The number of command line arguments.
 * @param argv The command line arguments.
 * @return An expected object containing the options or an exit code.
 */
[[nodiscard]] std::expected<options, int> parse_options(int argc, char const* const* argv);

