

#include "error_item.hpp"
#include <format>
#include <print>
#include <cassert>

namespace hk {


void error_item::print(std::vector<std::filesystem::path> const& upstream_paths) const
{
    println(stderr, "{}", to_string(upstream_paths));
}

[[nodiscard]] std::string error_item::to_string(std::vector<std::filesystem::path> const& upstream_paths) const
{
    auto const& upstream_path = upstream_paths.at(first().upstream_file);
    auto const& source_path = upstream_paths.at(0);

    auto m = std::format("{}: {}", first().to_string(upstream_paths), message());

    return m;
}

}