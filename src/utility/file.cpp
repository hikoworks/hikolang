
#include "file.hpp"
#include "file_ifstream.hpp"
#include "file_buffer.hpp"
#include <map>
#include <memory>
#include <cassert>

namespace hk {


[[nodiscard]] std::unique_ptr<file> make_file(std::filesystem::path path)
{
    assert(not path.empty());
    auto rel_path = path.relative_path();
    auto rel_path_str = rel_path.generic_string();

    if (rel_path_str.starts_with("tmp/hic-buffer-") and rel_path_str.ends_with(".bin")) {
        return std::make_unique<file_buffer>(std::move(path));
    } else {
        return std::make_unique<file_ifstream>(std::move(path));
    }
}   

} // namespace hk