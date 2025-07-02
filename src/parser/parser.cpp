
#include "parsers.hpp"
#include "tokenizer/tokenizer.hpp"
#include "utility/fixed_fifo.hpp"

namespace hl {


[[nodiscard]] parse_result_ptr<ast::node> parse(token_iterator& it, error_list& e)
{
    if (it[0] == "module") {
        return parse_module(it, e);
   // } else if (it[0] == "package") {
   //     return parse_package(it, e);
    } else {
        return e.add<"E0006: Expected 'module' or 'package' keyword">(it->first, it->last);
    }
}


[[nodiscard]] parse_result_ptr<ast::node> parse(hl::file_cursor& c)
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
    auto errors = error_list{};
    return parse(it, errors);
}

}
