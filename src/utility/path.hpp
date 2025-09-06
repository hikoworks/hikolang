

#pragma once

#include "interned.hpp"
#include <cstddef>
#include <limits>
#include <filesystem>
#include <system_error>
#include <cassert>
#include <expected>
#include <mutex>

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

/** Is subpath.
 * 
 * Make sure that @a path is a subpath inside of @a base.
 * 
 * @param path The file path a subpath inside @a base.
 * @param base A directory containing @a path.
 * @retval true If @a path is contained with @a base.
 * @retval false If @a path is outside @a base, or an error occured. 
 */
[[nodiscard]] bool is_subpath(std::filesystem::path const& path, std::filesystem::path const& base);

class scoped_temporary_directory {
public:
    ~scoped_temporary_directory();
    scoped_temporary_directory(std::string prefix = std::string{"tmp"});

    [[nodiscard]] std::filesystem::path const &path() const noexcept
    {
        return _path;
    }

private:
    inline static std::mutex _mutex;
    std::filesystem::path _path;
};

}
