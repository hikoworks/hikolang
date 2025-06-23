
#include "tokenizer.hpp"
#include "token_parsers.hpp"
#include "utility/module.hpp"
#include "utility/fixed_fifo.hpp"
#include <gsl/gsl>
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
    enum class state_type {
        normal,
        llvm_assembly,
    };

    state_type state = state_type::normal;
    while (not c.end_of_file()) {
        if (is_vertical_space(c[0], c[1])) {
            delegate.on_token(token{c.location(), '\n'});
            ++c;

        } else if (is_horizontal_space(c[0], c[1]) or is_ignoreable(c[0])) {
            ++c;
        
        } else if (auto t = parse_bracketed_string(c, state == state_type::llvm_assembly ? '{' : '\0')) {
            delegate.on_token(std::move(t).value());
            state = state_type::normal;

        } else if (is_separator(c[0]) or is_bracket(c[0])) {
            delegate.on_token(token{c.location(), gsl::narrow_cast<char>(c[0])});
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
            delegate.on_token(std::move(t).value());

        } else if (auto t = parse_positional_argument(c)) {
            delegate.on_token(std::move(t).value());

        } else if (c[0] == '$' and c[1] == '#') {
            auto r = token{c.location(), token::positional_argument};
            r.append('#');
            c += 2;
            r.last = c.location();
            delegate.on_token(std::move(r));

        } else if (auto t = parse_identifier(c)) {
            delegate.on_token(std::move(t).value());

            if (t == "llvm") {
                state = state_type::llvm_assembly;
            }

        } else if (auto t = parse_operator(c)) {
            delegate.on_token(std::move(t).value());

        } else if (c[0] == 0xfffd) {
            send_error(c, delegate, 1, "Invalid UTF-8 sequence; found a replacement character (U+FFFD).");

        } else if (c[0] >= 0xD800 and c[0] <= 0xDFFF) {
            send_error(c, delegate, 1, "Invalid surrogate code-point U+{:04x}.", static_cast<std::uint32_t>(c[0]));

        } else if (c[0] > 0x10FFFF) {
            send_error(c, delegate, 1, "Invalid code-point U+{:08x}.", static_cast<std::uint32_t>(c[0]));

        } else {
            send_error(c, delegate, 1, "Unexpected character U+{:04x} found.", static_cast<std::uint32_t>(c[0]));
        }
    }

    delegate.on_token(token{c.location(), '\0'});
}


void tokenize(hl::file_cursor& c, tokenize_delegate& delegate)
{
    struct delegate_type : tokenize_delegate {
        tokenize_delegate& delegate;

        fixed_fifo<token, 8> q = {};
        std::vector<char> bracket_stack = {};

        explicit delegate_type(tokenize_delegate& delegate) noexcept : delegate(delegate) {}

        void handle_one_token()
        {
            assert(not q.empty());

            auto t = q.pop_front();
            delegate.on_token(std::move(t));
        }

        void handle_all_tokens()
        {
            while (not q.empty()) {
                handle_one_token();
            }
        }

        void add_token(token t)
        {
            if (q.full()) {
                handle_one_token();
            }
            assert(not q.full());

            if (t == ';') {
                if (q.back() == ';') {
                    // Don't add duplicate semicolons.
                    return;
                
                } else if (q.back() == '{' or q.back() == '[' or q.back() == '(') {
                    // Don't add a semicolon after an opening bracket.
                    return;
                }

            } else if (t == ',') {
                if (q.back() == ',') {
                    q.push_back(t.make_error(t.last, "Unexpected duplicate comma ',' found."));
                    return;

                } else if (q.back() == '{' or q.back() == '[' or q.back() == '(') {
                    q.push_back(t.make_error(t.last, "Unexpected comma ',' found after an opening bracket."));
                    return;
                }

            } else if (t == '}' or t == ']' or t == ')' or t == '\0') {
                if (q.back() == ';' or q.back() == ',') {
                    // Remove trailing semicolons or commas before closing brackets.
                    q.pop_back();
                } 
            }

            q.push_back(std::move(t));
        }

        [[nodiscard]] bool is_keyword(token const& t) const noexcept
        {
            using namespace std::literals;

            constexpr std::array keywords = std::array{
                "struct"sv, "class"sv, "union"sv, "enum"sv, "function"sv
            };

            if (t.kind != token::identifier) {
                return false;
            }

            for (auto const& keyword : keywords) {
                if (t.text == keyword) {
                    return true;
                }
            }
            return false;
        }

        [[nodiscard]] bool forward(token const& t) const noexcept
        {
            return is_keyword(t);
        }

        [[nodiscard]] static bool ignore(token const& t) noexcept
        {
            return t == token::comment or t == token::line_directive;
        }

        void on_token(token t) override
        {
            auto doc_text = std::string{};

            if (ignore(t)) {
                // Drop tokens that we don't care about.
                return;

            } else if (forward(t)) {
                add_token(std::move(t));
                
            } else if (t == '{' or t == '[' or t == '(') {
                add_token(std::move(t));
                bracket_stack.push_back(t.simple_value());

            } else if (t == '}' or t == ']' or t == ')') {
                auto open_bracket = mirror_bracket(t.simple_value());

                if (bracket_stack.empty() or bracket_stack.back() != open_bracket) {
                    add_token(t.make_error(t.last, "Unmatched '}}' found; expected '{{'."));
                    return;
                }
                add_token(std::move(t));
                bracket_stack.pop_back();

            } else if (t == token::documentation) {
                doc_text = std::move(t.text);
                return; // Drop the token.

            } else if (t == token::back_documentation) {
                for (auto i = q.size(); i != 0; --i) {
                    auto & q_i = q[i - 1];
                    if (q_i == token::identifier) {
                        q_i.doc_text = std::move(t.text);
                        return; // Drop the token.
                    }
                }
                add_token(t.make_error(t.last, "Back documentation token found without a preceding identifier."));

            } else if (t == token::identifier) {
                t.doc_text = std::move(doc_text);
                add_token(std::move(t));

            } else if (t == '\n') {
                if (bracket_stack.empty() or bracket_stack.back() == '{') {
                    // Insert a semicolon at each line feed, when we are inside a block or at top-level.
                    add_token(token{t.first, ';'});
                }
                return; // Drop the token.

            } else if (t == '\0') {
                if (not bracket_stack.empty()) {
                    auto unmatched_bracket = bracket_stack.back();
                    bracket_stack.clear();
                    add_token(t.make_error(t.last, "Unmatched '{}' found; expected matching closing bracket.", unmatched_bracket));
                }

                add_token(std::move(t));
                handle_all_tokens();

            } else {
                // For all other tokens, just add them to the queue.
                add_token(std::move(t));
            }
        }
    };

    auto simple_delegate = delegate_type{delegate};
    return simple_tokenize(c, simple_delegate);
}

} // namespace hl
