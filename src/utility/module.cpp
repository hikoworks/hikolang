
#include "module.hpp"
#include <cassert>
#include <vector>
#include <cstddef>
#include <mutex>

namespace hk {

static std::vector<module> modules;
static std::mutex modules_mutex;

[[nodiscard]] std::string module::relative_path() const
{
    return _path.lexically_normal().string();
}

[[nodiscard]] module& make_module(std::filesystem::path path)
{
    auto const _ = std::scoped_lock{modules_mutex};
    modules.emplace_back(modules.size(), std::move(path));
    return modules.back();
}

[[nodiscard]] module& get_module(std::size_t module_id)
{
    auto const _ = std::scoped_lock{modules_mutex};
    return modules.at(module_id);
}

}