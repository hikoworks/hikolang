
#include "tokenizer.hpp"
#include "utility/file_buffer.hpp"
#include "utility/file_cursor.hpp"
#include "utility/unit_test.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(tokenizer_suite) 
{
    struct delegate_type : hk::tokenize_delegate {
        std::vector<hk::token> tokens;

        void on_token(hk::token t) override {
            tokens.push_back(t);
        }

    };

    static std::vector<hk::token> parse_tokens(std::string_view text)
    {
        auto path = hk::make_file_buffer(text);
        auto cursor = hk::file_cursor{path};
        auto delegate = delegate_type{};

        hk::tokenize(cursor, delegate);
        return std::move(delegate.tokens);
    }

    TEST_CASE(integer_12)
    {
        auto tokens = parse_tokens("12");
        REQUIRE(tokens.size() == 3);
        REQUIRE(tokens[0].kind == hk::token::integer_literal);
        REQUIRE(tokens[0].text == "12");
        REQUIRE(tokens[1] == ';');
        REQUIRE(tokens[2] == '\0');
    }

    TEST_CASE(integer_space_12)
    {
        auto tokens = parse_tokens(" 12");
        REQUIRE(tokens.size() == 3);
        REQUIRE(tokens[0].kind == hk::token::integer_literal);
        REQUIRE(tokens[0].text == "12");
        REQUIRE(tokens[1] == ';');
        REQUIRE(tokens[2] == '\0');
    }

    TEST_CASE(integer_12_space)
    {
        auto tokens = parse_tokens("12 ");
        REQUIRE(tokens.size() == 3);
        REQUIRE(tokens[0].kind == hk::token::integer_literal);
        REQUIRE(tokens[0].text == "12");
        REQUIRE(tokens[1] == ';');
        REQUIRE(tokens[2] == '\0');
    }

    TEST_CASE(integer_0b01)
    {
        auto tokens = parse_tokens("0b01");
        REQUIRE(tokens.size() == 3);
        REQUIRE(tokens[0].kind == hk::token::integer_literal);
        REQUIRE(tokens[0].text == "0b01");
        REQUIRE(tokens[1] == ';');
        REQUIRE(tokens[2] == '\0');
    }

    TEST_CASE(integer_0B10)
    {
        auto tokens = parse_tokens("0B10");
        REQUIRE(tokens.size() == 3);
        REQUIRE(tokens[0].kind == hk::token::integer_literal);
        REQUIRE(tokens[0].text == "0B10");
        REQUIRE(tokens[1] == ';');
        REQUIRE(tokens[2] == '\0');
    }

    TEST_CASE(integer_0x2a)
    {
        auto tokens = parse_tokens("0x2a");
        REQUIRE(tokens.size() == 3);
        REQUIRE(tokens[0].kind == hk::token::integer_literal);
        REQUIRE(tokens[0].text == "0x2a");
        REQUIRE(tokens[1] == ';');
        REQUIRE(tokens[2] == '\0');
    }

    TEST_CASE(integer_0X2A)
    {
        auto tokens = parse_tokens("0X2A");
        REQUIRE(tokens.size() == 3);
        REQUIRE(tokens[0].kind == hk::token::integer_literal);
        REQUIRE(tokens[0].text == "0X2A");
        REQUIRE(tokens[1] == ';');
        REQUIRE(tokens[2] == '\0');
    }

    TEST_CASE(short_file_file)
    {
        auto test_data_path = hk::test_data_path();
        auto path = test_data_path / "simple.hkm";
        auto cursor = hk::file_cursor{path};
        auto delegate = delegate_type{};

        hk::tokenize(cursor, delegate);
        auto tokens = delegate.tokens;
        REQUIRE(tokens.size() == 15);
        REQUIRE(tokens[0] == "module");
        REQUIRE(tokens[1] == "com");
        REQUIRE(tokens[2] == ".");
        REQUIRE(tokens[3] == "example");
        REQUIRE(tokens[4] == ".");
        REQUIRE(tokens[5] == "foo");
        REQUIRE(tokens[6] == "application");
        REQUIRE(tokens[7].text == "bar");
        REQUIRE(tokens[8] == ';');
        REQUIRE(tokens[9] == "import");
        REQUIRE(tokens[10] == "git");
        REQUIRE(tokens[11].text == "https://github.com/example/baz");
        REQUIRE(tokens[12].text == "main");
        REQUIRE(tokens[13] == ';');
        REQUIRE(tokens[14] == '\0');
    }

    TEST_CASE(short_file_content)
    {
        auto tokens = parse_tokens("\n\nmodule com.example.foo application \"bar\"\n\nimport git \"https://github.com/example/baz\" \"main\"\n");
        REQUIRE(tokens.size() == 15);
        REQUIRE(tokens[0] == "module");
        REQUIRE(tokens[1] == "com");
        REQUIRE(tokens[2] == ".");
        REQUIRE(tokens[3] == "example");
        REQUIRE(tokens[4] == ".");
        REQUIRE(tokens[5] == "foo");
        REQUIRE(tokens[6] == "application");
        REQUIRE(tokens[7].text == "bar");
        REQUIRE(tokens[8] == ';');
        REQUIRE(tokens[9] == "import");
        REQUIRE(tokens[10] == "git");
        REQUIRE(tokens[11].text == "https://github.com/example/baz");
        REQUIRE(tokens[12].text == "main");
        REQUIRE(tokens[13] == ';');
        REQUIRE(tokens[14] == '\0');
    }

};
