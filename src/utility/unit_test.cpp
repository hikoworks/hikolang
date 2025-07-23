
#include "unit_test.hpp"

namespace hk {

[[nodiscard]] std::filesystem::path test_data_path()
{
    auto p = std::filesystem::current_path();
    do {
        auto r = p / "test_data";
        if (std::filesystem::exists(r) and std::filesystem::is_directory(r)) {
            return r;
        }

        p = p.parent_path();
    } while (not p.relative_path().empty());

    throw std::runtime_error("Could not find test-data directory");
}

}
