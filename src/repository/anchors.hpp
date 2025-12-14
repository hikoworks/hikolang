
#include "repository.hpp"

namespace hk {

class anchors {
public:
    constexpr anchors() = default;

private:
    struct item_type {
        fqname name;
        semantic_version version;
        repository *repo;
    };

    std::vector<item_type> _items;
};

}