
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

        } else if (auto t = parse_scram_directive(c)) {
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

/** Helper class used by tokenize() for a second pass of tokenization.
 * 
 */
struct tokenize_delegate_helper : tokenize_delegate {
    tokenize_delegate& delegate;

    fixed_fifo<token, 8> q = {};
    std::vector<char> bracket_stack = {};

    explicit tokenize_delegate_helper(tokenize_delegate& delegate) noexcept : delegate(delegate) {}

    /** Send all tokens in the queue to the delegate.
     */
    void flush_tokens()
    {
        while (not q.empty()) {
            delegate.on_token(q.pop_front());
        }
    }

    /** Add token to the queue and send it to the delegate if the queue is full.
     * 
     * @param t The token to add to the queue.
     */
    void add_token(token t)
    {
        if (q.full()) {
            delegate.on_token(q.pop_front());
        }
        assert(not q.full());

        q.push_back(std::move(t));
    }

    /** Line-feeds are replaced with semicolon, or nothing.
     * 
     * @param t The token that was a line-feed.
     */
    void replace_line_feed_with_semicolon(token const& t)
    {
        if (not bracket_stack.empty() and bracket_stack.back() != '{') {
            // Insert a semicolon only if we are directly inside a block., or at top level.
            return;
        }

        if (not q.empty() and (q.back() == ';' or q.back() == '{')) {
            // Don't insert a semicolon where one should not be.
            return;
        }

        auto semicolon = token{t.first, ';'};
        semicolon.last = t.last;
        add_token(std::move(semicolon));
    }

    /** Check if an identifier is a keyword. 
     * 
     * This checks if an identifier is a well-known keyword in the language.
     * This is specifically so that documentation-comments will be associated
     * with non-keyword identifiers.
     * 
     * @param t The token to check.
     * @return true if the token is a keyword, false otherwise.
     */
    [[nodiscard]] static bool is_keyword(token const& t) noexcept
    {
        using namespace std::literals;

        /** Well-known keywords in the language.
        */
        constexpr std::array keywords = std::array{"struct"sv, "class"sv, "union"sv, "enum"sv, "function"sv};

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

    void on_token(token t) override
    {
        auto doc_text = std::string{};

        if (t == token::comment or t == token::line_directive) {
            // Drop tokens that we don't care about.
            return;

        } else if (is_keyword(t)) {
            // Keywords should not get documentation, so we handle this
            // earlier than other identifiers.
            add_token(std::move(t));

        } else if (t == '{' or t == '[' or t == '(') {
            add_token(std::move(t));
            bracket_stack.push_back(t.simple_value());

        } else if (t == '}' or t == ']' or t == ')') {
            auto const open_bracket = mirror_bracket(t.simple_value());

            if (bracket_stack.empty() or bracket_stack.back() != open_bracket) {
                add_token(t.make_error(
                    t.last,
                    "Unexpected '{}', missing open bracket '{}'.",
                    t.simple_value(),
                    gsl::narrow_cast<char>(open_bracket)));
                return;
            }
            add_token(std::move(t));
            bracket_stack.pop_back();

        } else if (t == token::documentation) {
            doc_text = std::move(t.text);
            return; // Drop the token.

        } else if (t == token::back_documentation) {
            for (auto i = q.size(); i != 0; --i) {
                auto& q_i = q[i - 1];
                if (q_i == token::identifier) {
                    q_i.doc_text = std::move(t.text);
                    return; // Drop the token.
                }
            }
            add_token(t.make_error(t.last, "Back documentation token found without a preceding identifier."));

        } else if (t == token::identifier) {
            if (not doc_text.empty()) {
                // If we have documentation text, associate it with the identifier.
                t.doc_text = std::move(doc_text);
                doc_text.clear();
            }
            add_token(std::move(t));

        } else if (t == '\n') {
            replace_line_feed_with_semicolon(t);
            return; // Drop the token.

        } else if (t == '\0') {
            while (not bracket_stack.empty()) {
                auto const unmatched_bracket = bracket_stack.back();
                add_token(
                    t.make_error(t.last, "Unmatched '{}' found; expected matching closing bracket.", unmatched_bracket));

                // Insert a matched closing bracket for each unmatched opening bracket.
                // So that the parser can continue longer during error recovery.
                auto const closing_bracket = mirror_bracket(unmatched_bracket);
                add_token(token{t.first, closing_bracket});
            }

            // Treat end-of-file as a possible line feed.
            replace_line_feed_with_semicolon(t);

            add_token(std::move(t));
            flush_tokens();

        } else {
            // For all other tokens, just add them to the queue.
            add_token(std::move(t));
        }
    }
};

void tokenize(hl::file_cursor& c, tokenize_delegate& delegate)
{
    auto simple_delegate = tokenize_delegate_helper{delegate};
    return simple_tokenize(c, simple_delegate);
}

} // namespace hl
