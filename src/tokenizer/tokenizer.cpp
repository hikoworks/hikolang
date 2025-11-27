
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


[[nodiscard]] static hk::generator<token> simple_tokenize(char const* p)
{
    enum class state_type {
        normal,
        llvm_assembly,
    };

    state_type state = state_type::normal;

    auto parse_llvm_string = [&](char const*& _p) {
        if (state == state_type::llvm_assembly) {
            return parse_bracketed_string(_p, '{', '}');
        } else {
            return token{};
        }
    };

    while (p[0] != '\0') {
        if (match<char, '\n', '\v', '\f'>(p[0])) {
            co_yield {p, '\n'};
            ++p;

        } else if (p[0] == '\r') {
            co_yield {p, '\n'};
            p += p[1] == '\n' ? 2 : 1;

        } else if (match<char, ' ', '\t'>(p[0])) {
            // Ignore white-space.
            ++p;
        
        } else if (auto t = parse_llvm_string(p)) {
            co_yield t;
            state = state_type::normal;

        } else if (match<char, '[', ']', '{', '}', '(', ')', ';', ','>(p[0])) {
            co_yield {p, p[0]};
            ++p;

        } else if (auto t = parse_line_comment(p)) {
            co_yield t;

        } else if (auto t = parse_block_comment(p)) {
            co_yield t;

        } else if (p[0] == '*' and p[1] == '/') {
            co_yield {p, 2, token::unexpected_end_of_comment_error};
            p += 2;

        } else if (auto t = parse_superscript_integer(p)) {
            co_yield t;

        } else if (auto t = parse_number(p)) {
            co_yield t;

        } else if (auto t = parse_string(p)) {
            co_yield t;

        } else if (auto t = parse_line_directive(p)) {
            co_yield t;

        } else if (auto t = parse_position_arg(p)) {
            co_yield t;

        } else if (auto t = parse_context_arg(p)) {
            co_yield t;

        } else if (auto t = parse_tag(p)) {
            co_yield t;

        } else if (auto t = parse_identifier(p)) {
            co_yield t;

            if (t == "llvm") {
                state = state_type::llvm_assembly;
            }

        } else if (auto t = parse_operator(p)) {
            co_yield t;

        } else {
            // Extracting an code-point is slow, so this is done last.
            auto const [cp, n] = get_cp(p);

            if (match<char32_t, U'\u0085', U'\u2028', U'\u2029'>(cp)) {
                co_yield {p, '\n'};

            } else if (cp == 0xfeff) {
                // Ignore BOM.

            } else if (match<char32_t, U'\u00a0', U'\u1680', U'\u202f', U'\u205f', U'\u3000'>(cp) or (cp >= 0x2000 and cp <= 0x200a)) {
                // Ignore white-space.
            
            } else if (cp == 0xfffd) {
                co_yield {p, n, token::invalid_replacement_character_error};

            } else if (cp >= 0xD800 and cp <= 0xDFFF) {
                co_yield {p, n, token::invalid_surrogate_code_point_error};

            } else if (cp > 0x10FFFF) {
                co_yield {p, n, token::invalid_code_point_error};

            } else {
                co_yield {p, n, token::unexpected_character_error};
            }

            p += n;
        }
    }

    co_yield {p, '\0'};
}

[[nodiscard]] hk::generator<token> tokenize(char const*& p, line_table &lines)
{
    fixed_fifo<token, 8> q = {};
    std::vector<char> bracket_stack = {};
    auto doc_text = std::string{};

    /** Line-feeds are replaced with semicolon, or nothing.
     *
     * @param t The token that was a line-feed.
     */
    auto replace_line_feed_with_semicolon = [&](token const& t) -> token {
        if (not bracket_stack.empty() and bracket_stack.back() != '{') {
            // Insert a semicolon only if we are directly inside a block., or at top level.
            return {};
        }

        if (q.empty() or q.back() == ';' or q.back() == '{') {
            // Don't add semicolon when there is a termination token on the queue.
            return {};
        }

        return token{t.data(), ';'};
    };

    for (auto t : simple_tokenize(p)) {
        if (t == token::comment) {
            // Drop comments.
            continue;

        } else if (t == token::line_directive) {
            auto [lineno, file_name] = t.line_value();
            if (lineno == 0) {
                if (auto r = q.push_back_overflow(t.make_error(token::invalid_line_directive_error))) {
                    co_yield std::move(r).value();
                }

            } else if (file_name.empty()) {
                lines.add(t.end(), lineno);
            } else {
                lines.add(t.end(), lineno, file_name);
            }
            continue;

        } else if (t == '{' or t == '[' or t == '(') {
            if (auto r = q.push_back_overflow(t)) {
                co_yield std::move(r).value();
            }
            bracket_stack.push_back(t.simple_value());

        } else if (t == '}' or t == ']' or t == ')') {
            auto const open_bracket = mirror_bracket(t.simple_value());

            if (bracket_stack.empty() or bracket_stack.back() != open_bracket) {
                if (auto r = q.push_back_overflow(t.make_error(token::missing_open_bracket_error))) {
                    co_yield std::move(r).value();
                }

                // Non recoverable error.
                break;
            }

            if (auto r = q.push_back_overflow(std::move(t))) {
                co_yield std::move(r).value();
            }
            bracket_stack.pop_back();

        } else if (t == '\n') {
            if (auto r = replace_line_feed_with_semicolon(t)) {
                if (auto r2 = q.push_back_overflow(r)) {
                    co_yield std::move(r2).value();
                }
            }
            // Drop the token.

        } else if (t == '\0') {
            while (not bracket_stack.empty()) {
                auto const unmatched_bracket = bracket_stack.back();

                if (auto r = q.push_back_overflow(t.make_error(token::unmatched_closing_bracket_error))) {
                    co_yield std::move(r).value();
                }

                // Insert a matched closing bracket for each unmatched opening bracket.
                // So that the parser can continue longer during error recovery.
                auto const closing_bracket = mirror_bracket(unmatched_bracket);
                if (auto r = q.push_back_overflow({t.data(), gsl::narrow_cast<char>(closing_bracket)})) {
                    co_yield std::move(r).value();
                }
            }

            // Treat end-of-file as a possible line feed.
            if (auto r = replace_line_feed_with_semicolon(t)) {
                if (auto r2 = q.push_back_overflow(r)) {
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
