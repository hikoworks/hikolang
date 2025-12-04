
#include "node.hpp"
#include "repository/repository.hpp"

namespace hk::ast {


std::unexpected<hkc_error> node::_add(hkc_error error, std::string message)
{
    return repository().errors.add(first, last, error, "{}", message);
}

}