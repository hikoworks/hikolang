
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

namespace hk {

template<typename... Args>
[[nodiscard]] static token make_error(hk::file_cursor& c, unsigned int advance, std::format_string<Args...> fmt, Args&&... args)
{
    auto r = token{c.location(), token::error};
    r.text = std::format(std::move(fmt), std::forward<Args>(args)...);
    c += advance;
    r.last = c.location();
    return r;
}

[[nodiscard]] static hk::generator<token> simple_tokenize(hk::file_cursor& c)
{
    enum class state_type {
        normal,
        llvm_assembly,
    };

    state_type state = state_type::normal;
    while (not c.end_of_file()) {
        if (is_vertical_space(c[0], c[1])) {
            co_yield {c.location(), '\n'};
            ++c;

        } else if (is_horizontal_space(c[0], c[1]) or is_ignoreable(c[0])) {
            ++c;

        } else if (auto t = parse_bracketed_string(c, state == state_type::llvm_assembly ? '{' : '\0')) {
            co_yield std::move(t).value();
            state = state_type::normal;

        } else if (is_separator(c[0]) or is_bracket(c[0])) {
            co_yield {c.location(), gsl::narrow_cast<char>(c[0])};
            ++c;

        } else if (auto t = parse_line_comment(c)) {
            co_yield std::move(t).value();

        } else if (auto t = parse_block_comment(c)) {
            co_yield std::move(t).value();

        } else if (c[0] == '*' and c[1] == '/') {
            co_yield make_error(c, 2, "Unexpected end of comment; found '*/' without a matching '/*'. ");

        } else if (auto t = parse_number(c)) {
            co_yield std::move(t).value();

        } else if (auto t = parse_string(c)) {
            co_yield std::move(t).value();

        } else if (auto t = parse_line_directive(c)) {
            co_yield std::move(t).value();

        } else if (auto t = parse_scram_directive(c)) {
            co_yield std::move(t).value();

        } else if (auto t = parse_position_arg(c)) {
            co_yield std::move(t).value();

        } else if (auto t = parse_context_arg(c)) {
            co_yield std::move(t).value();

        } else if (auto t = parse_tag(c)) {
            co_yield std::move(t).value();

        } else if (c[0] == '$' and c[1] == '#') {
            auto r = token{c.location(), token::position_arg};
            r.append('#');
            c += 2;
            r.last = c.location();
            co_yield std::move(r);

        } else if (auto t = parse_identifier(c)) {
            co_yield std::move(t).value();

            if (t == "llvm") {
                state = state_type::llvm_assembly;
            }

        } else if (auto t = parse_operator(c)) {
            co_yield std::move(t).value();

        } else if (c[0] == 0xfffd) {
            co_yield make_error(c, 1, "Invalid UTF-8 sequence; found a replacement character (U+FFFD).");

        } else if (c[0] >= 0xD800 and c[0] <= 0xDFFF) {
            co_yield make_error(c, 1, "Invalid surrogate code-point U+{:04x}.", static_cast<std::uint32_t>(c[0]));

        } else if (c[0] > 0x10FFFF) {
            co_yield make_error(c, 1, "Invalid code-point U+{:08x}.", static_cast<std::uint32_t>(c[0]));

        } else {
            co_yield make_error(c, 1, "Unexpected character U+{:04x} found.", static_cast<std::uint32_t>(c[0]));
        }
    }

    co_yield {c.location(), '\0'};
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

[[nodiscard]] hk::generator<token> tokenize(hk::file_cursor& c)
{
    fixed_fifo<token, 8> q = {};
    std::vector<char> bracket_stack = {};
    auto doc_text = std::string{};

    /** Line-feeds are replaced with semicolon, or nothing.
     *
     * @param t The token that was a line-feed.
     */
    auto replace_line_feed_with_semicolon = [&](token const& t) -> std::optional<token> {
        if (not bracket_stack.empty() and bracket_stack.back() != '{') {
            // Insert a semicolon only if we are directly inside a block., or at top level.
            return std::nullopt;
        }

        if (q.empty() or q.back() == ';' or q.back() == '{') {
            // Don't add semicolon when there is a termination token on the queue.
            return std::nullopt;
        }

        auto semicolon = token{t.first, ';'};
        semicolon.last = t.last;

        return semicolon;
    };

    for (auto t : simple_tokenize(c)) {
        if (t == token::comment or t == token::line_directive) {
            // Drop tokens that we don't care about.
            continue;

        } else if (is_keyword(t)) {
            // Keywords should not get documentation, so we handle this
            // earlier than other identifiers.
            if (auto r = q.push_back_overflow(std::move(t))) {
                co_yield std::move(r).value();
            }

        } else if (t == '{' or t == '[' or t == '(') {
            if (auto r = q.push_back_overflow(t)) {
                co_yield std::move(r).value();
            }
            bracket_stack.push_back(t.simple_value());

        } else if (t == '}' or t == ']' or t == ')') {
            auto const open_bracket = mirror_bracket(t.simple_value());

            if (bracket_stack.empty() or bracket_stack.back() != open_bracket) {
                auto error_token = token{t.make_error(
                    t.last,
                    "Unexpected '{}', missing open bracket '{}'.",
                    t.simple_value(),
                    gsl::narrow_cast<char>(open_bracket))};

                if (auto r = q.push_back_overflow(std::move(error_token))) {
                    co_yield std::move(r).value();
                }

                // Non recoverable error.
                break;
            }

            if (auto r = q.push_back_overflow(std::move(t))) {
                co_yield std::move(r).value();
            }
            bracket_stack.pop_back();

        } else if (t == token::documentation) {
            doc_text = std::move(t.text);
            // Drop the token.

        } else if (t == token::back_documentation) {
            for (auto i = q.size(); i != 0; --i) {
                auto& q_i = q[i - 1];
                if (q_i == token::identifier) {
                    q_i.doc_text = std::move(t.text);
                    continue; // Drop the token.
                }
            }
            auto error_token = t.make_error(t.last, "Back documentation token found without a preceding identifier.");

            if (auto r = q.push_back_overflow(std::move(error_token))) {
                co_yield std::move(r).value();
            }

        } else if (t == token::identifier) {
            if (not doc_text.empty()) {
                // If we have documentation text, associate it with the identifier.
                t.doc_text = std::move(doc_text);
                doc_text.clear();
            }

            if (auto r = q.push_back_overflow(std::move(t))) {
                co_yield std::move(r).value();
            }

        } else if (t == '\n') {
            if (auto r = replace_line_feed_with_semicolon(t)) {
                if (auto r2 = q.push_back_overflow(std::move(r).value())) {
                    co_yield std::move(r2).value();
                }
            }
            // Drop the token.

        } else if (t == '\0') {
            while (not bracket_stack.empty()) {
                auto const unmatched_bracket = bracket_stack.back();

                auto error_token = t.make_error(t.last, "Unmatched '{}' found; expected matching closing bracket.", unmatched_bracket);
                if (auto r = q.push_back_overflow(std::move(error_token))) {
                    co_yield std::move(r).value();
                }

                // Insert a matched closing bracket for each unmatched opening bracket.
                // So that the parser can continue longer during error recovery.
                auto const closing_bracket = mirror_bracket(unmatched_bracket);
                if (auto r = q.push_back_overflow({t.first, gsl::narrow_cast<char>(closing_bracket)})) {
                    co_yield std::move(r).value();
                }
            }

            // Treat end-of-file as a possible line feed.
            if (auto r = replace_line_feed_with_semicolon(t)) {
                if (auto r2 = q.push_back_overflow(std::move(r).value())) {
                    co_yield std::move(r2).value();
                }
            }

            if (auto r = q.push_back_overflow(std::move(t))) {
                co_yield std::move(r).value();
            }
            // We treated '\0' as an end of a bracketed section and as a
            // line-feed. If there is text after '\0'; handle this as normal
            // tokens.

        } else {
            // For all other tokens, just add them to the queue.
            if (auto r = q.push_back_overflow(std::move(t))) {
                co_yield std::move(r).value();
            }
        }
    }

    while (not q.empty()) {
        co_yield q.pop_front();
    }
}

} // namespace hk
