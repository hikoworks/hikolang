
#pragma once

#include "parse_result.hpp"
#include "parse_context.hpp"
#include "utility/fqname.hpp"
#include "error/error_list.hpp"
#include "tokenizer/token_vector.hpp"

namespace hk {

/** Parse a fully qualified name.
 *
 * @param it A token iterator.
 * @param e The list of errors.
 * @return A optional fully qualified name.
 */
[[nodiscard]] parse_result<fqname> parse_absolute_fqname(token_iterator& it, parse_context &ctx);

/** Parse a fully qualified name.
 *
 * @param it A token iterator.
 * @param e The list of errors.
 * @return A optional fully qualified name.
 */
[[nodiscard]] parse_result<fqname> parse_relative_fqname(token_iterator& it, parse_context &ctx);

}