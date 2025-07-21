
#pragma once

#include "token.hpp"
#include "tokenizer_context.hpp"
#include "utility/file_cursor.hpp"
#include "utility/char_category.hpp"
#include "utility/module.hpp"
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

/** A delegate interface for tokenization.
 * 
 * This interface is used to receive tokens from the tokenizer.
 */
struct tokenize_delegate {
    virtual ~tokenize_delegate() = default;

    /** Called when a token is produced by the tokenizer.
     * 
     * The last token will always be the end-of-file token '\0'.
     * 
     * @param t The token that was produced.
     */
    virtual void on_token(token t) = 0;
};

/** Tokenize all tokens pointed to by the file cursor.
 * 
 * This function will tokenize the input text and call the delegate for each token produced.
 * 
 * @param file_cursor The file cursor pointing to the text to tokenize.
 * @param delegate The delegate to call for each token produced.
 */
void tokenize(hk::file_cursor &file_cursor, tokenize_delegate &delegate, tokenizer_context &context);


} // namespace hk
