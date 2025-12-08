
#include "node.hpp"
#include "repository/source.hpp"

namespace hk::ast {


std::unexpected<hkc_error> node::_add(hkc_error error, std::string message)
{
    auto const [it, _] = source().errors().add(source().lines(), first, last, error, std::move(message));
    return std::unexpected{it->code()};
}

}