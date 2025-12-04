
#include "read_file.hpp"
#include <gsl/gsl>
#include <iostream>
#include <fstream>

namespace hk {

[[nodiscard]] std::expected<std::string, std::error_code> read_file(std::filesystem::path const& path, size_t extra_nul)
{
    auto ifs = std::ifstream(path, std::ios::in | std::ios::binary | std::ios::ate);
    if (ifs.bad() or ifs.fail()) {
        return std::unexpected{std::make_error_code(std::errc::io_error)};
    }

    // Open seeks to end.
    auto const file_size = ifs.tellg();
    if (file_size == -1) {
        return std::unexpected{std::make_error_code(std::errc::io_error)};
    }

    // Seek back to beginning.
    ifs.seekg(0, std::ios::beg);
    if (ifs.bad() or ifs.fail()) {
        return std::unexpected{std::make_error_code(std::errc::io_error)};
    }

    auto r = std::string{};
    r.resize_and_overwrite(
        gsl::narrow<size_t>(file_size) + extra_nul,
        [&](char* p, std::size_t s) {
            ifs.read(p, s - extra_nul);
            auto actual_size = ifs.gcount();
            std::memset(p + actual_size, 0, extra_nul);
            return actual_size + extra_nul;
        });

    // This checks the read() inside resize_and_overwrite().
    if (ifs.bad() or ifs.fail()) {
        return std::unexpected{std::make_error_code(std::errc::io_error)};
    }

    return r;
}

}