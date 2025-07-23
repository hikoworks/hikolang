
#include "parse_module.hpp"
#include "parsers.hpp"
#include "consume.hpp"
#include "ast/nodes.hpp"
#include "tokenizer/tokenizer.hpp"

namespace hk {


[[nodiscard]] ast::module_node_ptr parse_module(token_iterator& it, bool only_prologue)
{
    auto const first = it->first;
    auto r = std::make_unique<ast::module_node>(first);

    if (auto node = parse_module_declaration(it, r->errors)) {
        r->declaration = std::move(node).value();
    } else {
        return r;
    }

    while (true) {
        if (auto node = parse_import_declaration(it, r->errors)) {
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

[[nodiscard]] ast::module_node_ptr parse_module(hk::file_cursor& c, bool only_prologue)
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
    hk::tokenize(c, delegate);

    auto it = tokens.cbegin();
    auto m = parse_module(it, only_prologue);
    m->upstream_paths = c.upstream_paths();
    return m;
}

} // namespace hk