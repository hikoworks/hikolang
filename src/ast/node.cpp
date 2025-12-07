
#include "node.hpp"
#include "repository/source.hpp"

namespace hk::ast {


std::unexpected<hkc_error> node::_add(hkc_error error, std::string message)
{
    return std::unexpected{source().errors().add(source().lines(), first, last, error, std::move(message))};
}

}