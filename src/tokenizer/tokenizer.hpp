
#pragma once

#include "token.hpp"
#include "line_table.hpp"
#include "utility/generator.hpp"

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
