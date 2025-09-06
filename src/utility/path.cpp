

#include "path.hpp"
#include <format>
#include <chrono>
#include <system_error>
#include <print>

namespace hk {

[[nodiscard]] std::expected<std::filesystem::path, std::error_code>
absolute_to(std::filesystem::path const& path, std::filesystem::path const& base)
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
    if (ec) {
        return false;
    }

    auto const base_ = std::filesystem::canonical(base, ec);
    if (ec) {
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

scoped_temporary_directory::~scoped_temporary_directory()
{
    if (_path.empty()) {
        return;
    }

    auto const base_dir = std::filesystem::temp_directory_path();

    if (not is_subpath(_path, base_dir)) {
        std::println(
            stderr,
            "Security error: tried to remove scoped-temporary-directory '{}', while not being a subdirectory of '{}'.",
            _path.string(),
            base_dir.string());
        std::terminate();
    }

    if (not std::filesystem::exists(_path)) {
        return;
    }

    auto ec = std::error_code{};
    if (std::filesystem::remove_all(_path, ec) == static_cast<std::uintmax_t>(-1)) {
        std::println(stderr, "Could not remove scoped-temporary-directory '{}': {}", _path.string(), ec.message());
    }
}

scoped_temporary_directory::scoped_temporary_directory(std::string prefix)
{
    auto const _ = std::unique_lock(_mutex);

    if (prefix.empty()) {
        prefix = "tmp";
    }

    auto const base_dir = std::filesystem::temp_directory_path();
    auto count = 0uz;
    do {
        if (count++ > 10) {
            throw std::runtime_error("Too many tries to create scoped-temporary-directory");
        }

        auto const now = std::chrono::system_clock::now();
        auto const name = std::format("{0:}-{1:%Y%m%d}-{1:%H%M%S}-{2:}", prefix, now, count);
        _path = base_dir / name;
        if (std::filesystem::exists(_path)) {
            continue;
        }

        std::error_code ec;
        if (not std::filesystem::create_directory(_path, ec)) {
            if (ec and ec != std::errc::file_exists) {
                throw std::system_error(ec, std::format("Could not create directory: {}", _path.string()));
            }
            continue;
        }


    } while (false);
}

} // namespace hk