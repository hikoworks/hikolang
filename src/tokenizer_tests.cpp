
#include "tokenizer.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(tokenizer_suite) 
{
    struct delegate_type : hl::tokenize_delegate {
        std::vector<hl::token> tokens;

        void on_token(hl::token const& t) override {
            tokens.push_back(t);
        }

        void on_eof() override {
            // No action needed for EOF in this test.
        }
    };

    static hl::token parse_single_token(std::string_view module_text)
    {
        auto delegate = delegate_type{};
        auto result = hl::tokenize(1, module_text, delegate);
        REQUIRE(result.has_value());
        REQUIRE(delegate.tokens.size() == 1);
        return delegate.tokens.front();
    }

    TEST_CASE(integer)
    {
        auto token = parse_single_token("12");
        REQUIRE(token.kind == hl::token::integer_literal);
        REQUIRE(token.text == "12");

        token = parse_single_token(" 23");
        REQUIRE(token.kind == hl::token::integer_literal);
        REQUIRE(token.text == "23");

        token = parse_single_token("34 ");
        REQUIRE(token.kind == hl::token::integer_literal);
        REQUIRE(token.text == "34");

        token = parse_single_token("0b01");
        REQUIRE(token.kind == hl::token::integer_literal);
        REQUIRE(token.text == "0b01");

        token = parse_single_token("0B10");
        REQUIRE(token.kind == hl::token::integer_literal);
        REQUIRE(token.text == "0B10");

        token = parse_single_token("0x2a");
        REQUIRE(token.kind == hl::token::integer_literal);
        REQUIRE(token.text == "0x2a");

        token = parse_single_token("0X2A");
        REQUIRE(token.kind == hl::token::integer_literal);
        REQUIRE(token.text == "0X2A");


    }
};
