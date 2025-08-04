
#include "import_declaration_node.hpp"
#include "utility/sha.hpp"
#include "utility/base32.hpp"
#include <format>

namespace hk::ast {

[[nodiscard]] std::string import_declaration_node::path_hash() const
{
    auto const full_path = [this] {
        switch (kind) {
        case kind_type::git:
            return std::format("{}:{}", path, branch);
        case kind_type::zip:
            return path;
        case kind_type::mod:
            return static_cast<std::string>(name);
        case kind_type::lib:
            return path;
        }
        std::unreachable();
    }();

    auto const hash = sha256(full_path);
    return base32_encode(hash);
}

[[nodiscard]] std::string import_declaration_node::path_stem() const
{
    if (kind == kind_type::mod) {
        return static_cast<std::string>(name);
    }

    auto first = path.rfind('/');
    if (first == std::string::npos) {
        first = path.rfind('\\');
    }
    if (first == std::string::npos) {
        first = 0;
    } else {
        ++first;
    }

    auto last = path.rfind('.');
    if (last == std::string::npos or last <= first) {
        last = path.size();
    }
    
    return path.substr(first, last - first);
}

[[nodiscard]] std::string import_declaration_node::directory_name() const
{
    return std::format("{}-{}", path_stem(), path_hash().substr(0, 10));
}


}
