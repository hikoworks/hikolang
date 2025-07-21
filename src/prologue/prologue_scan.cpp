
#include "prologue_scan.hpp"

namespace hk {


std::vector<std::unique_ptr<ast::module_node>> prologue_scan_repository(std::filesystem::path const& path)
{
    auto r = std::vector<std::unique_ptr<ast::module_node>>{};

    auto first = std::filesystem::directory_entry{path};
    auto last = std::filesystem::directory_entry{path};
    for (auto it = first; it != last; ++it) {
        auto const& entry = *it;

        if (entry.path().filename().string().startswith(".")) {
            // Do not recursively scan hidden directories.
            if (entry.is_directory()) {
                it.disable_recursion_pending();
            }
            continue;
        }

        if (not entry.is_regular_file()) {
            continue;
        }

        if (entry.path().extension() != ".hiko") {
            continue;
        }

        r.push_back(parse_module(entry.path(), ast::module_node::state_type::prologue));
    }

    return r;
}