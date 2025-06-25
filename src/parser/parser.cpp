
#include "parser.hpp"
#include "tokenizer/tokenizer.hpp"
#include "utility/fixed_fifo.hpp"

namespace hl {


[[nodiscard]] std::unique_ptr<ast::node> parse(token_iterator& it, token_iterator it_end)
{
    if (auto root_node = parse_module(it, it_end)) {
        return root_node;
    } else if (auto root_node = parse_package(it, it_end)) {
        return root_node;
    }

    return nullptr;
}


[[nodiscard]] std::unique_ptr<ast::node> parse(hl::file_cursor& c)
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
    auto it_end = tokens.cend();
    return parse(it, it_end);
}

}
