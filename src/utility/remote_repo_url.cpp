
#include "remote_repo_url.hpp"
#include "sha.hpp"
#include "base32.hpp"
#include <format>
#include <utility>

namespace hk {


[[nodiscard]] std::string remote_repo_url::hash() const
{
    auto const text = [this] {
        switch (kind()) {
        case kind_type::empty:
            return std::string{};
        case kind_type::git:
            return std::format("{}:{}", path(), rev());
        case kind_type::zip:
            return path;
        }
        std::unreachable();
    }();

    return sha256(text);
}

[[nodiscard]] std::string remote_repo_url::stem() const
{
    if (kind() == kind_type::empty) {
        return std::string{};
    }

    auto first = path().rfind('/');
    if (first == std::string::npos) {
        first = path().rfind('\\');
    }
    if (first == std::string::npos) {
        first = 0;
    } else {
        ++first;
    }

    auto last = path().rfind('.');
    if (last == std::string::npos or last <= first) {
        last = path().size();
    }
    
    return path().substr(first, last - first);
}

[[nodiscard]] std::string remote_repo_url::directory() const
{
    auto short_hash = base32_encode(hash()).substr(0, 10);
    return std::format("{}-{}", stem(), short_hash);
}

}