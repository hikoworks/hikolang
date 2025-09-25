
#include "module.hpp"
#include <cassert>

namespace hk {

module_t::module_t(std::filesystem::path path) : path(std::move(path))
{
    assert(std::filesystem::canonical(this->path) == this->path);
}

}

