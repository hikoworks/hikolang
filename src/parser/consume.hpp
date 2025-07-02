
#pragma once

#include "parsers.hpp"

namespace hl {

/** Consume the rest of the current statement at the token iterator. 
 * 
 *  This function will consume tokens until it reaches a semicolon (`;`) or the end of the input.
 * 
 * It is used to skip over the rest of a statement when an error occurs; useful
 * for the parser to continue parsing without getting stuck on an incomplete statement.
 * 
 * @param [in,out] it The token iterator to consume from.
 *                    It will be advanced to the end of the statement or the end of the input.
 */
void consume_rest_of_statement(token_iterator& it)
{
    while (true) {
        if (*it == '\0') {
            // If we reach the end of the input, we stop consuming.
            return;
        } else if (*it == ';') {
            // Consume the semicolon and stop.
            ++it;
            return;
        }
        ++it;
    }
}

}