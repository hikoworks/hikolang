
#pragma once

#include "utility/fixed_string.hpp"
#include "error_code.hpp"

namespace hk {

template<fixed_string Fmt>
struct make_error {
    constexpr static decltype(Fmt) fmt = Fmt;
    constexpr static error_code code = error_code{fmt};
};


}
