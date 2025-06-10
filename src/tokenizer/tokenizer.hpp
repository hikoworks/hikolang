
#pragma once

#include "token.hpp"
#include "utility/file_cursor.hpp"
#include "utility/char_category.hpp"
#include "utility/utf8.hpp"
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

namespace hl {


struct tokenize_delegate {
    virtual ~tokenize_delegate() = default;

    virtual void on_token(token const &t) = 0;
};

void tokenize(hl::file_cursor &file_cursor, tokenize_delegate &delegate);


} // namespace hl
