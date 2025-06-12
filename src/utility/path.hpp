

#pragma once

#include <cstddef>
#include <limits>
#include <filesystem>

namespace hl {

/** A unique identifier for a path.
 * 
 * Internally we use identifiers to paths, to reduce memory usage and
 * improve performance when dealing with many paths.
 */
enum class path_id : std::size_t {
    invalid = std::numeric_limits<std::size_t>::max()
};

/** Get a unique identifier for a path.
 * 
 * The path will be normalized to an absolute path.
 * 
 * @note The path does not need to exist on the filesystem.
 * @param path A path, if relative compared to the current working directory.
 * @return A unique identifier for the path.
 */
[[nodiscard]] path_id get_path_id(std::filesystem::path path);

/** Get a unique identifier for a path relative to another path.
 * 
 * The path will be normalized to an absolute path.
 * 
 * @note The path does not need to exist on the filesystem.
 * @param path A path, if relative compared to @a relative_to.
 * @param relative_to A path to resolve the relative path against.
 * @return A unique identifier for the path.
 */
[[nodiscard]] path_id get_path_id(std::filesystem::path path, path_id relative_to);

/** Get a unique identifier for a buffer_file.
 * 
 * This path does not need to exist on the filesystem.
 * The path will have the following format:
 *  - "/tmp/hic-buffer-<unique_id>.bin"
 */
[[nodiscard]] path_id get_path_id();

/** Get the absolute normalized path for a given path identifier.
 * 
 * @param id The path identifier.
 * @return The absolute normalized path for the given identifier.
 */
[[nodiscard]] std::filesystem::path const& get_path(path_id id);

}
