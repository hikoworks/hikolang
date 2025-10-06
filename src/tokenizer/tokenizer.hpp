
#pragma once

#include "token.hpp"
#include "file_cursor.hpp"
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
 * @param file_cursor The file cursor pointing to the text to tokenize.
 * @param delegate The delegate to call for each token produced.
 */
[[nodiscard]] hk::generator<token> tokenize(hk::file_cursor &file_cursor);


} // namespace hk
