
#pragma once

#include <filesystem>
#include <string>
#include <cstddef>

namespace hl {

class module {
public:
    module() noexcept = default;
    module(std::size_t id, std::filesystem::path const& path) noexcept : _id(id), _path(path) {}

    /** Path to the module file.
     *
     * This is the canonical path to the module file. This is done
     * so that we can match module files parsed from different locations.
     */
    [[nodiscard]] std::string relative_path() const;

private:
    size_t _id = 0;
    std::filesystem::path _path;
};

[[nodiscard]] module& make_module(std::filesystem::path const& path);

[[nodiscard]] module& get_module(std::size_t module_id);

}
