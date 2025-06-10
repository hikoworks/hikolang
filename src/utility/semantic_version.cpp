
#include "semantic_version.hpp"
#include "char_category.hpp"

namespace hl {

semantic_version::semantic_version(std::string_view version) :
    major(std::numeric_limits<std::size_t>::max()),
    minor(std::numeric_limits<std::size_t>::max()),
    patch(std::numeric_limits<std::size_t>::max())
{
    auto it = version.begin();
    auto const end = version.end();

    if (it == end) {
        return;
    }

    if (*it == 'v') {
        ++it; // Skip the 'v'
    }

    if (it == end) {
        return;
    }

    major = 0;
    while (true) {
        if (it == end) {
            return;

        } else if (is_digit(*it)) {
            major *= 10;
            major += *it - '0';
            ++it;

        } else if (*it == '.') {
            ++it;
            goto minor_version;

        } else {
            throw std::invalid_argument("Invalid semantic version format");
        }
    }

minor_version:
    minor = 0;

    while (true) {
        if (it == end) {
            return;

        } else if (*it == '*' and minor == 0) {
            minor = std::numeric_limits<std::size_t>::max();
            ++it;
            if (it == end or *it != '.') {
                throw std::invalid_argument("Invalid semantic version format");
            }
            goto patch_version;

        } else if (is_digit(*it)) {
            minor *= 10;
            minor += *it - '0';
            ++it;

        } else if (*it == '.') {
            ++it;
            goto patch_version;

        } else {
            throw std::invalid_argument("Invalid semantic version format");
        }
    }

patch_version:
    patch = 0;
    while (true) {
        if (it == end) {
            return;

        } else if (*it == '*' and patch == 0) {
            patch = std::numeric_limits<std::size_t>::max();
            ++it;
            if (it != end) {
                throw std::invalid_argument("Invalid semantic version format");
            }
            return;
            
        } else if (is_digit(*it)) {
            patch *= 10;
            patch += *it - '0';
            ++it;

        } else {
            throw std::invalid_argument("Invalid semantic version format");
        }
    }
}

}
