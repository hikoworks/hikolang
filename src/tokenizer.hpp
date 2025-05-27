
#ifndef HL_TOKENIZER_HPP
#define HL_TOKENIZER_HPP

#include "token.hpp"
#include "char_category.hpp"
#include "char_lookahead.hpp"
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

class tokenizer {
public:
    struct delegate_type {
        virtual ~delegate_type() = default;
        virtual void on_token(token const& t) = 0;
    };

    tokenizer(size_t module_id, std::string_view module_text) noexcept;

    tokenizer(tokenizer&&) noexcept = delete;
    tokenizer(tokenizer const&) noexcept = delete;
    tokenizer& operator=(tokenizer&&) noexcept = delete;
    tokenizer& operator=(tokenizer const&) noexcept = delete;

    [[nodiscard]] std::expected<void, std::string> tokenize(delegate_type& delegate);

private:
    character_lookahead<3> _lookahead = {};
    char const* _ptr = nullptr;
    char const* _end = nullptr;
    std::size_t _module_id = 0;
    std::size_t _line_nr = 0;
    std::size_t _column_nr = 0;

    bool decode_utf8() noexcept;

    char32_t advance() noexcept;

    [[nodiscard]] token make_token(token::kind_type kind) const;

    [[nodiscard]] token make_character_token(token::kind_type kind, char c) const;

    [[nodiscard]] token make_character_token(token::kind_type kind, char32_t c) const;

    template<typename... Args>
    [[nodiscard]] std::unexpected<std::string> make_error(std::format_string<Args...> fmt, Args&&... args) const
    {
        auto error_message = std::format(fmt, std::forward<Args>(args)...);
        auto const& m = get_module(_module_id);
        auto message = std::format("{}:{}:{}: {}", m.relative_path(), _line_nr + 1, _column_nr + 1, std::move(error_message));
        return std::unexpected{std::move(message)};
    }

    [[nodiscard]] std::unexpected<std::string> make_unexpected_character_error(char32_t cp, char const* start) const;


    [[nodiscard]] std::expected<token, std::string> parse_string();

    [[nodiscard]] std::expected<token, std::string> parse_identifier();

    [[nodiscard]] std::expected<token, std::string> parse_operator();

    [[nodiscard]] std::expected<token, std::string> parse_number();
};

struct tokenize_delegate {
    virtual ~tokenize_delegate() = default;

    virtual void on_token(token const &t) = 0;
};

[[nodiscard]] std::expected<void, std::string> tokenize(size_t module_id, std::string_view module_text, tokenize_delegate &delegate);


} // namespace hl

#endif // HL_TOKENIZER_HPP
