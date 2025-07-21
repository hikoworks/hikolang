
#include "strings.hpp"

namespace hk {

[[nodiscard]] std::vector<std::string> split(std::string_view str, char delimiter)
{
    auto r = std::vector<std::string>{};

    if (str.empty()) 
    {
        return r;
    }

    auto start = 0uz;
    auto end = str.find(delimiter);
    
    while (end != std::string_view::npos) {
        r.emplace_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }
    
    r.emplace_back(str.substr(start));
    return r;
}

}
