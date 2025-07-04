
#include "parsers.hpp"
#include "consume.hpp"
#include "ast/nodes.hpp"
#include "tokenizer/tokenizer.hpp"

namespace hl {


[[nodiscard]] parse_result_ptr<ast::module_node> parse_module(token_iterator& it, error_list& e, bool only_prologue)
{
    auto const first = it->first;
    auto r = std::make_unique<ast::module_node>(first);

    if (auto node = parse_module_declaration(it, e)) {
        r->declaration = std::move(node).value();
    } else if (node.error()) {
        return std::unexpected{node.error()};
    } else {
        return tokens_did_not_match;
    }

    while (true) {
        if (auto node = parse_import_declaration(it, e)) {
            r->imports.push_back(std::move(node).value());
        } else if (node.error()) {
            // Parsing can continue on failure.
            consume_rest_of_statement(it);
        } else {
            // Something else than a `import` declaration was found.
            break;
        }
    }

    if (only_prologue) {
        // If we are only parsing the prologue, we stop here.
        auto const last = it->first;
        r->last = last;
        return r;
    }

    auto const last = it->first;
    r->last = last;
    return r;
}

[[nodiscard]] parse_result_ptr<ast::module_node> parse_module(hl::file_cursor& c, error_list &e, bool only_prologue)
{
    class delegate_type : public tokenize_delegate {
    public:
        delegate_type(std::vector<token> &tokens) : _tokens(std::addressof(tokens)) {}

        void on_token(token t) override
        {
            if (t == '\0') {
                // If the token is an end-of-file token, we append extra tokens to make sure
                // the parser can always look ahead without checking for end-of-file.
                for (auto i = 0uz; i != 8; ++i) {
                    _tokens->push_back(t);
                }
            }

            _tokens->push_back(std::move(t));
        }

    private:
        std::vector<token>* _tokens;
    };

    auto tokens = std::vector<token>{};
    auto delegate = delegate_type(tokens);
    hl::tokenize(c, delegate);

    auto it = tokens.cbegin();
    return parse_module(it, e, only_prologue);
}

} // namespace hl