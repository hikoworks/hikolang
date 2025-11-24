
#pragma once

#include <optional>
#include <system_error>
#include <string>
#include <filesystem>

namespace hk {

/** Read a file into a string.
 * 
 * @param path The path to the file to read.
 * @param extra_nul Append a number of extra NUL characters.
 * @return The file read into a file, or unable to read the file.
 */
[[nodiscard]] std::optional<std::string> read_file(std::filesystem::path const& path, size_t extra_nul = 0);

}