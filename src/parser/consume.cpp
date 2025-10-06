
#include "consume.hpp"

namespace hk {

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