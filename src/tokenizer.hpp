
#ifndef HL_TOKENIZER_HPP
#define HL_TOKENIZER_HPP

#include "token.hpp"
#include "char_category.hpp"
#include "char_lookahead.hpp"
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

class tokenizer {
public:
    struct delegate_type {
        virtual ~delegate_type() = default;
        virtual void on_token(token const& t) = 0;
        virtual void on_eof() = 0;
    };

    tokenizer(size_t module_id, std::string_view module_text) noexcept;

    tokenizer(tokenizer&&) noexcept = delete;
    tokenizer(tokenizer const&) noexcept = delete;
    tokenizer& operator=(tokenizer&&) noexcept = delete;
    tokenizer& operator=(tokenizer const&) noexcept = delete;

    [[nodiscard]] std::expected<void, std::string> tokenize(delegate_type& delegate);

private:
    character_lookahead<8> _lookahead = {};
    char const* _ptr = nullptr;
    char const* _end = nullptr;
    std::size_t _module_id = 0;

    /** The file that is part of the module, which is the source of the tokens.
     * This is used to report errors in the actual source, in for exampl
     */
    std::size_t _file_id = 0;
    std::size_t _line_nr = 0;
    std::size_t _column_nr = 0;

    /** Check if the tokenizer has processed all bytes in the input.
     * 
     */
    [[nodiscard]] bool end_of_file() const noexcept;

    /** Advance one Unicode code-point.
     * 
     * It has the following effects:
     *  - It will pop the first value from the lookahead buffer.
     *  - It will update the line and column number based on the code-point.
     *  - It will decode the next code-point from the input buffer.
     */
    void advance() noexcept;

    template<typename... Args>
    [[nodiscard]] token make_token(Args&&... args) const
    {
        return token{_module_id, _file_id, _line_nr, _column_nr, std::forward<Args>(args)...};
    }

    template<typename... Args>
    [[nodiscard]] std::unexpected<std::string> make_error(std::format_string<Args...> fmt, Args&&... args) const
    {
        auto error_message = std::format(fmt, std::forward<Args>(args)...);
        auto const& m = get_module(_module_id);
        auto message = std::format("{}:{}:{}: {}", m.relative_path(), _line_nr + 1, _column_nr + 1, std::move(error_message));
        return std::unexpected{std::move(message)};
    }

    [[nodiscard]] std::unexpected<std::string> make_unexpected_character_error(char32_t cp, char const* start) const;


    [[nodiscard]] maybe_expected<token, std::string> parse_string();
    [[nodiscard]] maybe_expected<token, std::string> parse_identifier();
    [[nodiscard]] maybe_expected<token, std::string> parse_operator();
    [[nodiscard]] maybe_expected<token, std::string> parse_version();
    [[nodiscard]] maybe_expected<token, std::string> parse_number();
    [[nodiscard]] maybe_expected<token, std::string> parse_line_comment();
    [[nodiscard]] maybe_expected<token, std::string> parse_block_comment();
    [[nodiscard]] maybe_expected<token, std::string> parse_positional_argument();
    [[nodiscard]] maybe_expected<token, std::string> parse_line_directive();
};

struct tokenize_delegate {
    virtual ~tokenize_delegate() = default;

    virtual void on_token(token const &t) = 0;
    virtual void on_eof() = 0;
};

[[nodiscard]] std::expected<void, std::string> tokenize(size_t module_id, std::string_view module_text, tokenize_delegate &delegate);


} // namespace hl

#endif // HL_TOKENIZER_HPP
