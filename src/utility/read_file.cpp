
#include "read_file.hpp"
#include <iostream>
#include <fstream>

namespace hk {

[[nodiscard]] std::optional<std::string> read_file(std::filesystem::path const& path, size_t extra_nul)
{
    auto ifs = std::ifstream(path, std::ios::in | std::ios::binary | std::ios::ate);
    if (ifs.bad() or ifs.fail()) {
        return std::nullopt;
    }

    // Open seeks to end.
    auto const file_size = ifs.tellg();
    if (file_size == -1) {
        return std::nullopt;
    }

    // Seek back to beginning.
    ifs.seekg(0, std::ios::beg);
    if (ifs.bad() or ifs.fail()) {
        return std::nullopt;
    }

    auto r = std::string{};
    r.resize_and_overwrite(
        file_size + extra_nul,
        [&](char* p, std::size_t s) {
            ifs.read(p, s);
            auto actual_size = ifs.gcount();
            std::memset(p + actual_size, 0, extra_nul);
            return actual_size + extra_nul;
        });

    // This checks the read() inside resize_and_overwrite().
    if (ifs.bad() or ifs.fail()) {
        return std::nullopt;
    }

    return r;
}

}