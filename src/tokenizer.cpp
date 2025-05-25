
#include "tokenizer.hpp"

namespace hl {

struct parse_token_context {
    std::size_t line_nr = 0;
    std::size_t column_nr = 0;
    std::size_t module_id = 0;

    std::string text = {};

    [[nodiscard]] constexpr token make_token(token::kind_type kind, char c) const noexcept
    {
        return token{kind, module_id, line_nr, column_nr, c};
    }

    constexpr void advance(std::string_view::const_iterator& it, size_t count = 1) noexcept
    {
        for (size_t i = 0; i != count; ++i) {
            auto const c = *it++;

            if (c == '\n') {
                ++line_nr;
                column_nr = 0;
            } else if ((c & 0xc0) == 0x80) {
                // Skip UTF-8 continuation code-unit.
            } else {
                ++column_nr;
            }
        }
    }
};

class character_lookahead {
public:
    struct value_type {
        char const* start = nullptr;
        std::uint32_t cp = 0;
    };

    void pop_front() noexcept
    {
        if (_size > 0) {
            std::shift_right(_values.begin(), _values.end(), 1);
            _values.last() = {};
            --_size;
        }
    }

    void push_back(char const* start, std::uint32_t cp) noexcept
    {
        if (_size == _values.size()) {
            // If we are full, pop the front value.
            pop_front();
        }
        _values[_size++] = {start, cp};
    }

    bool decode_utf8(char const*& ptr, char const* const end) noexcept
    {
        while (ptr != end and _size != _values.size()) {
            auto const prev_ptr = ptr;
            auto const cp = decode_utf8_code_point(ptr, end);
            push_back(prev_ptr, cp);
        }
        return _size != 0;
    }

    [[nodiscard]] constexpr value_type const& operator[](std::size_t index) const noexcept
    {
        assert(index < _size);
        return _values[index];
    }

private:
    std::array<value_type, 2> _values = {};
    std::size_t _size = 0;
};

struct tokenize_simple_delegate {
    virtual ~tokenize_simple_delegate() = default;
    virtual void on_token(token const& t) = 0;
};

static std::expected<token, std::string>
parse_number(parse_token_context& context, character_lookahead& lookahead, char const*& ptr, char const* const end)
{
    enum class state_type {
        start,
        found_sign,
        found_e,
        found_esign,

        zero_prefix,
        radix,
        integer_part,
        fraction_part,
        exponent_part
    };

    auto state = state_type::start;

    auto r = token{token::integer_literal, context.module_id, context.line_nr, context.column_nr, ""};
    auto const start_ptr = ptr;

    while (lookahead.decode_utf8(ptr, end)) {
        auto const cp = lookahead[0].cp;

        switch (state) {
        case state_type::start:
            if (cp == '+' or cp == '-') {
                state = state_type::found_sign;
            } else if (cp == '0') {
                state = state_type::zero_prefix;
            } else if (cp >= '1' and cp <= '9') {
                state = state_type::integer_part;
            } else if (cp == '.') {
                state = state_type::fraction_part;
            } else {
                return std::unexpected{context.make_unexpected_character_error(cp, lookahead[0].start, end)};
            }
            break;

        case state_type::found_sign:
            if (cp == '0') {
                state = state_type::zero_prefix;
            } else if (cp >= '1' and cp <= '9') {
                state = state_type::integer_part;
            } else if (cp == '.') {
                state = state_type::fraction_part;
            } else {
                return std::unexpected{context.make_unexpected_character_error(cp, lookahead[0].start, end)};
            }

        case state_type::zero_prefix:
            if (cp == 'b' or cp == 'B' or cp == 'o' or cp == 'O' or cp = 'd' or cp == 'D' or cp == 'x' or cp == 'X') {
                state = state_type::integer_part;
            } else if (cp >= '0' and cp <= '9') {
                state = state_type::integer_part;
            } else if (cp == '.') {
                state = state_type::fraction_part;
            } else if (cp == 'e' or cp == 'E') {
                state = state_type::found_e;
            } else {
                r.text = std::string_view{start_ptr, lookahead[0].start};
                context.horizontal_advance();
                lookahead.pop_front();
                return r;
            }

        case state_type::integer_part:
            if (cp >= '0' and cp <= '9') {
                state = state_type::integer_part;
            } else if (cp == '.') {
                state = state_type::fraction_part;
            } else if (cp == 'e' or cp == 'E') {
                state = state_type::found_e;
            } else {
                r.text = std::string_view{start_ptr, lookahead[0].start};
                context.horizontal_advance();
                lookahead.pop_front();
                return r;
            }

        case state_type::fraction_part:
            if (cp >= '0' and cp <= '9') {
                state = state_type::fraction_part;
            } else if (cp == 'e' or cp == 'E') {
                state = state_type::found_e;
            } else {
                r.kind = token::float_literal;
                r.text = std::string_view{start_ptr, lookahead[0].start};
                context.horizontal_advance();
                lookahead.pop_front();
                return r;
            }

        case state_type::found_e:
            if (cp == '+' or cp == '-' or (cp >= '0' and cp <= '9')) {
                state = state_type::exponent_part;
            } else {
                return std::unexpected{context.make_unexpected_character_error(cp, lookahead[0].start, end)};
            }

        case state_type::exponent_part:
            if (cp >= '0' and cp <= '9') {
                state = state_type::exponent_part;
            } else {
                r.kind = token::float_literal;
                r.text = std::string_view{start_ptr, lookahead[0].start};
                context.horizontal_advance();
                lookahead.pop_front();
                return r;
            }
        }

        context.horizontal_advance();
        lookahead.pop_front();
    }
}

static std::expected<void, std::string>
tokenize_simple(parse_token_context& context, char const* ptr, char const* const end, tokenize_simple_delegate& delegate)
{
    auto found_cr = false;
    auto lookahead = character_lookahead{};

    while (lookahead.decode_utf8(ptr, end)) {
        auto const cp = lookahead[0].cp;
        auto const cp_start = lookahead[0].start;
        auto const next_cp = lookahead[1].cp;

        if (found_cr and cp != '\n') {
            // Treat a solo CR as a LF. Solo CR used to be old style MacOS line
            // endings. Insert a LF token before the next token.
            found_cr = false;
            delegate.on_token(context.make_token(token::line_feed, '\r'));
        }

        if (cp == '\r') {
            // Found a CR, treat the same as horizontal space. But remember.
            found_cr = true;
            context.advance_horizontal();
            lookahead.pop_front();

        } else if (is_vertical_space(cp)) {
            f(context.make_token(token::line_feed, cp));
            context.advance_vertical();
            lookahead.pop_front();

        } else if (is_horizontal_space(cp)) {
            context.advance_horizontal();
            lookahead.pop_front();

        } else if (is_ignoreable(cp)) {
            lookahead.pop_front();

        } else if (is_bracket(cp)) {
            delegate.on_token(context.make_token(token::bracket, cp));
            context.advance_horizontal();
            lookahead.pop_front();

        } else if (is_separator(cp)) {
            delegate.on_token(context.make_token(token::seperator, cp));
            context.advance_horizontal();
            lookahead.pop_front();

        } else if (is_digit(cp) or (cp == '.' and is_digit(next_cp))) {
            if (auto const optional_token = parse_number(context, lookahead)) {
                delegate.on_token(*optional_token);
            } else {
                return std::unexpected{optional_token.error()};
            }

        } else if (
            cp == '"' or cp == '\'' or cp == '`' or (cp == 'r' and (next_cp == '"' or next_cp == '\'' or next_cp == '`'))) {
            if (auto const optional_token = parse_string(context, lookahead)) {
                delegate.on_token(*optional_token);
            } else {
                return std::unexpected{optional_token.error()};
            }

        } else if (is_identifier_start(cp)) {
            if (auto const optional_token = parse_identifier(context, lookahead)) {
                delegate.on_token(*optional_token);
            } else {
                return std::unexpected{optional_token.error()};
            }

        } else if (is_pattern_syntax(cp)) {
            if (auto const optional_token = parse_operator(context, lookahead)) {
                delegate.on_token(*optional_token);
            } else {
                return std::unexpected{optional_token.error()};
            }

        } else if (cp == std::to_underlying(decode_utf8_error::continuation_byte)) {
            return std::unexpected{context.make_error(
                "Invalid UTF-8 sequence; found a lone continuation byte {}.", display_utf8_sequence(cp_start, end))};

        } else if (cp == std::to_underlying(decode_utf8_error::missing_continuation_byte)) {
            return std::unexpected{context.make_error(
                "Invalid UTF-8 sequence; found a non-continuation byte in a multi-byte sequence {}.",
                display_utf8_sequence(cp_start, end))};

        } else if (cp == std::to_underlying(decode_utf8_error::buffer_overrun)) {
            return std::unexpected{context.make_error(
                "Invalid UTF-8 sequence; multi-byte sequence continues beyond end of buffer {}.",
                display_utf8_sequence(cp_start, end))};

        } else if (cp == std::to_underlying(decode_utf8_error::overlong_encoding)) {
            return std::unexpected{
                context.make_error("Invalid UTF-8 sequence; overlong encoding {}.", display_utf8_sequence(cp_start, end))};

        } else if (cp == std::to_underlying(decode_utf8_error::out_of_range)) {
            return std::unexpected{
                context.make_error("Invalid UTF-8 sequence; code-point out of range {}.", display_utf8_sequence(cp_start, end))};

        } else if (cp == std::to_underlying(decode_utf8_error::surrogate)) {
            return std::unexpected{
                context.make_error("Invalid UTF-8 sequence; surrogate value {}.", display_utf8_sequence(cp_start, end))};

        } else {
            return std::unexpected{context.make_error("Unexpected character: U+{:06x}", cp)};
        }
    }
}

std::expected<void, std::string> tokenize(size_t module_id, std::string_view module_text, tokenize_delegate& delegate)
{
    auto context = parse_token_context{module_id};
    auto const ptr = module_text.data();
    auto const end = ptr + module_text.size();

    struct simple_delegate_type : tokenize_simple_delegate {
        tokenize_delegate& delegate;

        void on_token(token const& t) override
        {
            delegate.on_token(t);
        }
    };

    auto simple_delegate = simple_delegate_type{delegate};
    return tokenize_simple(context, ptr, end, simple_delegate);
}
