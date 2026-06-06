
#pragma once

#include "token.hpp"

namespace hk {

[[nodiscard]] token tokenize_line_directive(char const*& p);

}
