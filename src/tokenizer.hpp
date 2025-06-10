
#ifndef HL_TOKENIZER_HPP
#define HL_TOKENIZER_HPP

#include "token.hpp"
#include "file_cursor.hpp"
#include "char_category.hpp"
#include "maybe_expected.hpp"
#include "utf8.hpp"
#include "module.hpp"
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

namespace hl {


struct tokenize_delegate {
    virtual ~tokenize_delegate() = default;

    virtual void on_token(token const &t) = 0;
};

void tokenize(hl::file_cursor &file_cursor, tokenize_delegate &delegate);


} // namespace hl

#endif // HL_TOKENIZER_HPP
