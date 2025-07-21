

#pragma once

#include "interned.hpp"
#include <cstddef>
#include <limits>
#include <filesystem>
#include <system_error>
#include <cassert>
#include <expected>

namespace hk {

/** Convert a relative path to an absolute path based on a base path.
 * 
 * Construct an canonical absolute path from a relative path and a base path:
 *  - If the path is already absolute, it is returned as is.
 *  - If the base path is absolute, the function combines the base path with the relative path.
 *  - If the base path is relative, it is first converted to an absolute path
 *    using the current working directory, and then combined with the relative path.
 * 
 * @param path The relative path to convert.
 * @param base The base path to use for conversion.
 * @param ec An error code to indicate success or failure.
 * @return The absolute path as a std::filesystem::path object.
 */
[[nodiscard]] std::expected<std::filesystem::path, std::error_code> absolute_to(
    std::filesystem::path const& path, std::filesystem::path const& base);

/** A unique identifier for a filesystem path.
 * 
 * This class is used to intern filesystem paths, allowing for efficient
 * comparison and storage of paths in a way that avoids duplication.
 */
using path_id = interned<std::filesystem::path>;

}
