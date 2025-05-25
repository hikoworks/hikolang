
#include "token.hpp"
#include "char_category.hpp"
#include "utf8.hpp"
#include <concepts>
#include <string>
#include <string_view>
#include <expected>
#include <cstdint>
#include <cstddef>
#include <array>
#include <algorithm>

namespace hl {

struct tokenize_delegate {
    virtual ~tokenize_delegate() = default;

    virtual void on_token(token const &t) = 0;
};

std::expected<void, std::string> tokenize(size_t module_id, std::string_view module_text, tokenize_delegate &delegate);


} // namespace hl
