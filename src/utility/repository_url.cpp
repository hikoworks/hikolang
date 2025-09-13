
#include "repository_url.hpp"
#include "sha.hpp"
#include "base32.hpp"
#include <format>
#include <utility>

namespace hk {


[[nodiscard]] std::array<char, 32> repository_url::hash() const
{
    auto const text = [this] {
        switch (kind()) {
        case repository_type::none:
            return std::string{};
        case repository_type::git:
            return std::format("{}:{}", url(), rev());
        case repository_type::zip:
            return url();
        }
        std::unreachable();
    }();

    return sha256(text);
}

[[nodiscard]] std::string repository_url::stem() const
{
    if (kind() == repository_type::none) {
        return std::string{};
    }

    auto first = url().rfind('/');
    if (first == std::string::npos) {
        first = url().rfind('\\');
    }
    if (first == std::string::npos) {
        first = 0;
    } else {
        ++first;
    }

    auto last = url().rfind('.');
    if (last == std::string::npos or last <= first) {
        last = url().size();
    }
    
    return url().substr(first, last - first);
}

[[nodiscard]] std::string repository_url::directory() const
{
    auto short_hash = base32_encode(hash()).substr(0, 10);
    return std::format("{}-{}", stem(), short_hash);
}

}