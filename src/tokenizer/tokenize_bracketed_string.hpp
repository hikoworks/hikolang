
#pragma once

#include "token.hpp"

namespace hk {

[[nodiscard]] token tokenize_bracketed_string(
    char const*& p, char open_bracket, char close_bracket);

}
