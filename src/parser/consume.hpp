
#pragma once

#include "tokenizer/token_vector.hpp"

namespace hk {

/** Consume the rest of the current statement at the token iterator. 
 * 
 * This function will consume tokens until it reaches a semicolon (`;`) or the end of the input.
 * 
 * It is used to skip over the rest of a statement when an error occurs; useful
 * for the parser to continue parsing without getting stuck on an incomplete statement.
 * 
 * @param [in,out] it The token iterator to consume from.
 *                    It will be advanced to the end of the statement or the end of the input.
 */
void consume_rest_of_statement(token_iterator& it);

}