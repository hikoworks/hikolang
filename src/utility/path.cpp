

#include "path.hpp"

namespace hk {

[[nodiscard]] std::expected<std::filesystem::path, std::error_code> absolute_to(
    std::filesystem::path const& path, std::filesystem::path const& base)
{
    auto ec = std::error_code{};

    if (path.is_absolute()) {
        return path;

    } else if (base.is_absolute()) {
        if (auto r = std::filesystem::canonical(base / path, ec); not ec) {
            return r;
        } else {
            return std::unexpected{ec};
        }

    } else if (auto base_abs = std::filesystem::absolute(base, ec); not ec) {
        if (auto r = std::filesystem::canonical(base_abs / path, ec); not ec) {
            return r;
        } else {
            return std::unexpected{ec};
        }

    } else {
        return std::unexpected{ec};
    }
}

[[nodiscard]] bool is_subpath(std::filesystem::path const& path, std::filesystem::path const& base)
{
    auto ec = std::error_code{};

    auto const path_ = std::filesystem::canonical(path, ec);
    if (not ec) {
        return false;
    }

    auto const base_ = std::filesystem::canonical(path, ec);
    if (not ec) {
        return false;
    }

    auto it = base_.begin();
    auto jt = path_.begin();

    for (; it != base_.end(); ++it, ++jt) {
        if (jt == path_.end()) {
            return false;
        }

        if (*it != *jt) {
            return false;
        }
    }
    return true;
}

}