
#include "tokenizer.hpp"
#include "token_parsers.hpp"
#include "utility/utf8.hpp"
#include "utility/module.hpp"
#include <cassert>
#include <cstdint>
#include <cstddef>
#include <array>
#include <algorithm>
#include <format>
#include <expected>

namespace hl {

template<typename... Args>
static void
send_error(hl::file_cursor& c, tokenize_delegate& delegate, unsigned int advance, std::format_string<Args...> fmt, Args&&... args)
{
    auto r = token{c.location(), token::error};
    r.text = std::format(std::move(fmt), std::forward<Args>(args)...);
    c += advance;
    r.last = c.location();
    delegate.on_token(std::move(r));
}

static void simple_tokenize(hl::file_cursor& c, tokenize_delegate& delegate)
{
    auto found_cr = false;

    while (not c.end_of_file()) {
        if (found_cr and c[0] != '\n') {
            // Treat a solo CR as a LF. Solo CR used to be old style MacOS line
            // endings. Insert a LF token before the next token.
            found_cr = false;
            delegate.on_token(token{c.location(), token::line_feed, '\n'});
        }

        if (c[0] == '\r') {
            // Found a CR, treat the same as horizontal space. But remember.
            found_cr = true;
            ++c;

        } else if (is_vertical_space(c[0])) {
            delegate.on_token(token{c.location(), token::line_feed, '\n'});
            ++c;

        } else if (is_horizontal_space(c[0]) or is_ignoreable(c[0])) {
            ++c;

        } else if (is_bracket(c[0])) {
            delegate.on_token(token{c.location(), token::bracket, c[0]});
            ++c;

        } else if (is_separator(c[0])) {
            delegate.on_token(token{c.location(), token::seperator, c[0]});
            ++c;

        } else if (auto t = parse_line_comment(c)) {
            delegate.on_token(std::move(t).value());

        } else if (auto t = parse_block_comment(c)) {
            delegate.on_token(std::move(t).value());

        } else if (c[0] == '*' and c[1] == '/') {
            send_error(c, delegate, 2, "Unexpected end of comment; found '*/' without a matching '/*'. ");

        } else if (auto t = parse_number(c)) {
            delegate.on_token(std::move(t).value());

        } else if (auto t = parse_string(c)) {
            delegate.on_token(std::move(t).value());

        } else if (auto t = parse_line_directive(c)) {
            if (t->kind == token::error) {
                // Only pass on errors from the line directive parser.
                // As the directive is purely handled by the tokenizer.
                delegate.on_token(std::move(t).value());
            }

        } else if (auto t = parse_positional_argument(c)) {
            delegate.on_token(std::move(t).value());

        } else if (c[0] == '$' and c[1] == '#') {
            auto r = token{c.location(), token::positional_argument, '#'};
            c += 2;
            r.last = c.location();
            delegate.on_token(std::move(r));

        } else if (auto t = parse_identifier(c)) {
            delegate.on_token(std::move(t).value());

        } else if (auto t = parse_operator(c)) {
            delegate.on_token(std::move(t).value());

        } else if (c[0] == 0xfffd) {
            send_error(c, delegate, 1, "Invalid UTF-8 sequence; found a replacement character (U+FFFD).");

        } else if (c[0] >= 0xD800 and c[0] <= 0xDFFF) {
            send_error(c, delegate, 1, "Invalid surrogate code-point U+{:04x}.", static_cast<std::uint32_t>(c[0]));

        } else if (c[0] > 0x10FFFF) {
            send_error(c, delegate, 1, "Invalid code-point U+{:08x}.", static_cast<std::uint32_t>(c[0]));
        }
    }

    delegate.on_token(token{c.location(), token::end_of_file, '\0'});
}


void tokenize(hl::file_cursor& c, tokenize_delegate& delegate)
{
    struct simple_delegate_type : tokenize_delegate {
        tokenize_delegate& delegate;

        explicit simple_delegate_type(tokenize_delegate& delegate) noexcept : delegate(delegate) {}

        void on_token(token const& t) override
        {
            delegate.on_token(t);
        }
    };

    auto simple_delegate = simple_delegate_type{delegate};
    return simple_tokenize(c, simple_delegate);
}

} // namespace hl
