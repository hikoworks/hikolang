
#include "prologue_scan.hpp"
#include "utility/vector_set.hpp"

namespace hk {


[[nodiscard]] std::vector<std::unique_ptr<ast::module_node>> prologue_scan_repository(std::filesystem::path const& path)
{
    auto r = std::vector<std::unique_ptr<ast::module_node>>{};

    auto first = std::filesystem::recursive_directory_iterator{path};
    auto last = std::filesystem::recursive_directory_iterator{};

    auto visited = vector_set<std::filesystem::path>{};
    for (auto it = first; it != last; ++it) {
        auto const& entry = *it;

        if (entry.path().filename().string().starts_with(".")) {
            // Do not recursively scan hidden directories.
            if (entry.is_directory()) {
                it.disable_recursion_pending();
            }
            continue;
        }

        if (not visited.add(std::filesystem::canonical(entry.path()))) {
            // This directory has already been visited; a potential symlink loop.
            it.disable_recursion_pending();
            continue;
        }
            

        if (not entry.is_regular_file()) {
            continue;
        }

        if (entry.path().extension() != ".hkm") {
            continue;
        }

        auto cursor = file_cursor(entry.path());
        if (auto node = parse_module(cursor, errors, true)) {

            r.push_back(std::move(node()).value());
        } else if (node.is_error()) {


        } else {

        }
    }

    return r;
}

}
