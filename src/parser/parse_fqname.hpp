
#pragma once

#include "parse_result.hpp"
#include "utility/fqname.hpp"
#include "error/error_list.hpp"
#include "tokenizer/token_vector.hpp"

namespace hk {

/** Parse a fully qualified name.
 * 
 * @param it A token iterator.
 * @param e The list of errors.
 * @param leading_dot Parse fully qualified names that can start with a dot.
 * @return A optional fully qualified name.
 */
[[nodiscard]] parse_result<fqname> parse_fqname(token_iterator& it, error_list& e, bool leading_dot = true);

}