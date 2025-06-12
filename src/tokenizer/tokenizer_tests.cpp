
#include "tokenizer.hpp"
#include "utility/file_buffer.hpp"
#include "utility/file_cursor.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(tokenizer_suite) 
{
    struct delegate_type : hl::tokenize_delegate {
        std::vector<hl::token> tokens;

        void on_token(hl::token const& t) override {
            tokens.push_back(t);
        }

    };

    static std::vector<hl::token> parse_tokens(std::string_view text)
    {
        auto path_id = hl::make_file_buffer(text);
        auto cursor = hl::file_cursor{path_id, path_id};
        auto delegate = delegate_type{};

        hl::tokenize(cursor, delegate);
        return std::move(delegate.tokens);
    }

    TEST_CASE(integer_12)
    {
        auto tokens = parse_tokens("12");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].kind == hl::token::integer_literal);
        REQUIRE(tokens[0].text == "12");
        REQUIRE(tokens[1].kind == hl::token::end_of_file);
    }

    TEST_CASE(integer_space_12)
    {
        auto tokens = parse_tokens(" 12");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].kind == hl::token::integer_literal);
        REQUIRE(tokens[0].text == "12");
        REQUIRE(tokens[1].kind == hl::token::end_of_file);
    }

    TEST_CASE(integer_12_space)
    {
        auto tokens = parse_tokens("12 ");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].kind == hl::token::integer_literal);
        REQUIRE(tokens[0].text == "12");
        REQUIRE(tokens[1].kind == hl::token::end_of_file);
    }

    TEST_CASE(integer_0b01)
    {
        auto tokens = parse_tokens("0b01");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].kind == hl::token::integer_literal);
        REQUIRE(tokens[0].text == "0b01");
        REQUIRE(tokens[1].kind == hl::token::end_of_file);
    }

    TEST_CASE(integer_0B10)
    {
        auto tokens = parse_tokens("0B10");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].kind == hl::token::integer_literal);
        REQUIRE(tokens[0].text == "0B10");
        REQUIRE(tokens[1].kind == hl::token::end_of_file);
    }

    TEST_CASE(integer_0x2a)
    {
        auto tokens = parse_tokens("0x2a");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].kind == hl::token::integer_literal);
        REQUIRE(tokens[0].text == "0x2a");
        REQUIRE(tokens[1].kind == hl::token::end_of_file);
    }

    TEST_CASE(integer_0X2A)
    {
        auto tokens = parse_tokens("0X2A");
        REQUIRE(tokens.size() == 2);
        REQUIRE(tokens[0].kind == hl::token::integer_literal);
        REQUIRE(tokens[0].text == "0X2A");
        REQUIRE(tokens[1].kind == hl::token::end_of_file);
    }
};
