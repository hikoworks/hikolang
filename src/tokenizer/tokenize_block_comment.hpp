
#pragma once

#include "token.hpp"

namespace hk {

[[nodiscard]] token tokenize_block_comment(char const*& p);

}
