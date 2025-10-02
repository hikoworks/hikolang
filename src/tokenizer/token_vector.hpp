
#pragma once

#include "token.hpp"
#include "utility/lazy_vector.hpp"

namespace hk {

using token_vector = lazy_vector<generator<token>::const_iterator, std::default_sentinel_t, token>;
using token_iterator = token_vector::const_iterator;

}
