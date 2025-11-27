
#pragma once

#include "token.hpp"
#include "line_table.hpp"
#include "char_category.hpp"
#include "utility/module.hpp"
#include "utility/generator.hpp"
#include <concepts>
#include <string>
#include <string_view>
#include <expected>
#include <cstdint>
#include <cstddef>
#include <array>
#include <algorithm>
#include <format>
#include <cassert>

namespace hk {

/** Tokenize all tokens pointed to by the file cursor.
 * 
 * This function will tokenize the input text and call the delegate for each token produced.
 * 
 * @param p Pointer to source code text which has at least 8 nul terminating the text.
 * @param lines A line table that is updated for the #line directive.
 */
[[nodiscard]] hk::generator<token> tokenize(char const* p, line_table &lines);


} // namespace hk
