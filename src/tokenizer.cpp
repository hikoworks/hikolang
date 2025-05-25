
#include "tokenizer.hpp"

namespace hl {

template<size_t N>
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
        assert(start != nullptr);

        if (_size == _values.size()) {
            // If we are full, pop the front value.
            pop_front();
        }
        _values[_size++] = {start, cp};
    }

    bool decode_utf8(char const*& ptr, char const* const end) noexcept
    {
        assert(_ptr != nullptr);
        assert(_end != nullptr);

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
    std::array<value_type, N> _values = {};
    std::size_t _size = 0;
};

class tokenizer {
public:
    struct delegate_type {
        virtual ~delegate_type() = default;
        virtual void on_token(token const& t) = 0;
    };

    tokenizer(size_t module_id, std::string_view module_text) noexcept :
        _lookahead(), _ptr(module_text.data()), _end(_ptr + module_text.size()), _module_id(module_id), _line_nr(0), _column_nr(0)
    {
    }

    tokenizer(tokenizer&&) noexcept = delete;
    tokenizer(tokenizer const&) noexcept = delete;
    tokenizer& operator=(tokenizer&&) noexcept = delete;
    tokenizer& operator=(tokenizer const&) noexcept = delete;

    [[nodiscard]] std::expected<void, std::string> tokenize(delegate_type& delegate)
    {
        assert(_line_nr == 0);
        assert(_column_nr == 0);
        assert(_ptr != nullptr);
        assert(_end != nullptr);

        auto found_cr = false;

        while (context.decode_utf8()) {
            auto const cp = context[0].cp;
            auto const cp_ptr = context[0].start;
            auto const next_cp = context[1].cp;
            auto const next_next_cp = context[2].cp;

            if (found_cr and cp != '\n') {
                // Treat a solo CR as a LF. Solo CR used to be old style MacOS line
                // endings. Insert a LF token before the next token.
                found_cr = false;
                delegate.on_token(context.make_character_token(token::line_feed, '\r'));
            }

            if (cp == '\r') {
                // Found a CR, treat the same as horizontal space. But remember.
                found_cr = true;
                context.advance();

            } else if (is_vertical_space(cp)) {
                delegate.on_token(context.make_character_token(token::line_feed, '\n'));
                context.advance();

            } else if (is_horizontal_space(cp)) {
                context.advance();

            } else if (is_ignoreable(cp)) {
                context.advance();

            } else if (is_bracket(cp)) {
                delegate.on_token(context.make_character_token(token::bracket, cp));
                context.advance();

            } else if (is_separator(cp)) {
                delegate.on_token(context.make_character_token(token::seperator, cp));
                context.advance();

            } else if (
                is_digit(cp) or (cp == '.' and is_digit(next_cp)) or
                ((cp == '-' or cp == '+') and (is_digit(next_cp) or (next_cp == '.' and is_digit(next_next_cp))))) {
                // `-`, `+`, `.` and `..` are valid operators, distinguish them from numbers.
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

            } else {
                return context.make_unexpected_character_error(cp, cp_ptr);
            }
        }
    }

private:
    character_lookahead<3> _lookahead = {};
    char const* _ptr = nullptr;
    char const* _end = nullptr;
    std::size_t _module_id = 0;
    std::size_t _line_nr = 0;
    std::size_t _column_nr = 0;

    bool decode_utf8() noexcept
    {
        assert(_ptr != nullptr);
        assert(_end != nullptr);
        return _lookahead.decode_utf8(_ptr, _end);
    }

    [[nodiscard]] character_lookahead::value_type const& operator[](std::size_t index) const
    {
        return _lookahead[index];
    }

    char32_t advance() noexcept
    {
        auto const cp = _lookahead[0].cp;
        _lookahead.pop_front();

        if (is_vertical_space(cp)) {
            ++_line_nr;
            _column_nr = 0;
        } else {
            ++_column_nr;
        }

        return cp;
    }

    [[nodiscard]] token make_token(token::kind_type kind) const
    {
        return token{kind, _module_id, _line_nr, _column_nr};
    }

    [[nodiscard]] token make_character_token(token::kind_type kind, char32_t c) const
    {
        assert(c < 128);
        return token{kind, module_id, line_nr, column_nr, static_cast<char>(c)};
    }

    template<typename... Args>
    [[nodiscard]] std::unexpected<std::string> make_error(std::format_string<Args...> fmt, Args&&... args) const
    {
        auto error_message = std::format(fmt, std::forward<Args>(args)...);
        auto const& m = get_module(module_id);
        auto message = std::format("{}:{}:{}: {}", m.relative_path(), line_nr + 1, column_nr + 1, std::move(error_message));
        return std::unexpected{std::move(message)};
    }

    [[nodiscard]] std::unexpected<std::string> make_unexpected_character_error(std::uint32_t cp, char const* start) const
    {
        switch (cp) {
        case std::to_underlying(decode_utf8_error::continuation_byte):
            return make_error("Invalid UTF-8 sequence; found a lone continuation byte {}.", display_utf8_sequence(start, end));

        case std::to_underlying(decode_utf8_error::missing_continuation_byte):
            return make_error(
                "Invalid UTF-8 sequence; found a non-continuation byte in a multi-byte sequence {}.",
                display_utf8_sequence(cp_start, end));

        case std::to_underlying(decode_utf8_error::buffer_overrun):
            return make_error(
                "Invalid UTF-8 sequence; multi-byte sequence continues beyond end of buffer {}.",
                display_utf8_sequence(cp_start, end));

        case std::to_underlying(decode_utf8_error::overlong_encoding):
            return make_error("Invalid UTF-8 sequence; overlong encoding {}.", display_utf8_sequence(cp_start, end));

        case std::to_underlying(decode_utf8_error::out_of_range):
            return make_error("Invalid UTF-8 sequence; code-point out of range {}.", display_utf8_sequence(cp_start, end));

        case std::to_underlying(decode_utf8_error::surrogate):
            return make_error("Invalid UTF-8 sequence; surrogate value {}.", display_utf8_sequence(cp_start, end));

        default:
            return make_error("Unexpected character: '{}' U+{:06x}.", static_cast<char32_t>(cp), cp);
        }
    }


    [[nodiscard]] std::expected<token, std::string> parse_string()
    {
        auto r = make_token{token::string_literal};

        auto const is_raw_string = [&] {
            if (_lookahead[0].cp == 'r') {
                advance();
                return true;
            }
            return false;
        }();

        auto const quote_char = _lookahead[0].cp;
        advance();

        auto escape = false;
        while (lookahead.decode_utf8(ptr, end)) {
            auto const cp = _lookahead[0].cp;

            if (cp == quote_char and not escape) {
                // End of string literal.
                r.push_back(quote_char);
                advance();
                return r;

            } else if (cp == '\\') {
                // Escape sequence.
                advance();
                if (not lookahead.decode_utf8(ptr, end)) {
                    return make_error("Unterminated string literal.");
                }
            }

            advance();
        }

        return make_error("Unterminated string literal.");
    }

    [[nodiscard]] std::expected<token, std::string> parse_identifier()
    {
        auto r = make_token{token::identifier};
        auto const start_ptr = _lookahead[0].start;

        // Skip the first character, which is guaranteed to be an identifier start.
        advance();

        while (lookahead.decode_utf8(ptr, end)) {
            auto const cp = _lookahead[0].cp;

            if (not is_identifier_continue(cp) and cp != '_') {
                // End of identifier.
                r.text = std::string_view{start_ptr, _lookahead[0].start};
                return r;
            }

            advance();
        }

        r.text = std::string_view{start_ptr, _end};
        return r;
    }

    [[nodiscard]] std::expected<token, std::string> parse_operator()
    {
        auto r = make_token{token::operator_};
        auto const start_ptr = _lookahead[0].start;

        // Skip the first character, which is guaranteed to be a pattern syntax character.
        advance();

        while (lookahead.decode_utf8(ptr, end)) {
            if (not is_pattern_syntax(_lookahead[0].cp)) {
                // End of operator.
                r.text = std::string_view{start_ptr, _lookahead[0].start};
                return r;
            }

            advance();
        }

        r.text = std::string_view{start_ptr, _end};
        return r;
    }

    [[nodiscard]] std::expected<token, std::string> parse_number()
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

        auto r = make_token{token::integer_literal};
        auto const start_ptr = _lookahead[0].start;

        while (lookahead.decode_utf8(ptr, end)) {
            auto const cp = _lookahead[0].cp;
            auto const cp_ptr = _lookahead[0].start;

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
                    return make_unexpected_character_error(cp, cp_ptr);
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
                    return make_unexpected_character_error(cp, cp_ptr);
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
                    r.text = std::string_view{start_ptr, cp_ptr};
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
                    r.text = std::string_view{start_ptr, cp_ptr};
                    return r;
                }

            case state_type::fraction_part:
                if (cp >= '0' and cp <= '9') {
                    state = state_type::fraction_part;
                } else if (cp == 'e' or cp == 'E') {
                    state = state_type::found_e;
                } else {
                    r.kind = token::float_literal;
                    r.text = std::string_view{start_ptr, cp_ptr};
                    return r;
                }

            case state_type::found_e:
                if (cp == '+' or cp == '-' or (cp >= '0' and cp <= '9')) {
                    state = state_type::exponent_part;
                } else {
                    return make_unexpected_character_error(cp, cp_ptr);
                }

            case state_type::exponent_part:
                if (cp >= '0' and cp <= '9') {
                    state = state_type::exponent_part;
                } else {
                    r.kind = token::float_literal;
                    r.text = std::string_view{start_ptr, cp_ptr};
                    return r;
                }
            }

            advance();
        }
    }
};

[[nodiscard]] std::expected<void, std::string>
tokenize(size_t module_id, std::string_view module_text, tokenize_delegate& delegate)
{
    auto context = tokenizer{module_id};
    auto const ptr = module_text.data();
    auto const end = ptr + module_text.size();

    struct simple_delegate_type : tokenizer::delegate_type {
        tokenize_delegate& delegate;

        void on_token(token const& t) override
        {
            delegate.on_token(t);
        }
    };

    auto simple_delegate = simple_delegate_type{delegate};
    return tokenizer.tokenize(simple_delegate);
}
