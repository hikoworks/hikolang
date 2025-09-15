
#include "tokenizer.hpp"
#include "utility/file_buffer.hpp"
#include "utility/file_cursor.hpp"
#include "utility/lazy_vector.hpp"
#include "test_utilities/paths.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(tokenizer_suite) {
#define parse_tokens(output, text) \
    auto path = hk::make_file_buffer(text); \
    auto cursor = hk::file_cursor{path}; \
    auto token_generator = hk::tokenize(cursor); \
    auto output = hk::lazy_vector{token_generator.cbegin(), token_generator.cend()}

TEST_CASE(integer_12)
{
    parse_tokens(tokens, "12");
    REQUIRE(tokens[0].kind == hk::token::integer_literal);
    REQUIRE(tokens[0].text == "12");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(integer_space_12)
{
    parse_tokens(tokens, " 12");
    REQUIRE(tokens[0].kind == hk::token::integer_literal);
    REQUIRE(tokens[0].text == "12");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(integer_12_space)
{
    parse_tokens(tokens, "12 ");
    REQUIRE(tokens[0].kind == hk::token::integer_literal);
    REQUIRE(tokens[0].text == "12");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(integer_0b01)
{
    parse_tokens(tokens, "0b01");
    REQUIRE(tokens[0].kind == hk::token::integer_literal);
    REQUIRE(tokens[0].text == "0b01");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(integer_0B10)
{
    parse_tokens(tokens, "0B10");
    REQUIRE(tokens[0].kind == hk::token::integer_literal);
    REQUIRE(tokens[0].text == "0B10");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(integer_0x2a)
{
    parse_tokens(tokens, "0x2a");
    REQUIRE(tokens[0].kind == hk::token::integer_literal);
    REQUIRE(tokens[0].text == "0x2a");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(integer_0X2A)
{
    parse_tokens(tokens, "0X2A");
    REQUIRE(tokens[0].kind == hk::token::integer_literal);
    REQUIRE(tokens[0].text == "0X2A");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(short_file_file)
{
    auto test_data_path = test::test_data_path();
    auto path = test_data_path / "simple.hkm";
    auto cursor = hk::file_cursor{path};

    auto token_generator = hk::tokenize(cursor);
    auto tokens = hk::lazy_vector{token_generator.cbegin(), token_generator.cend()};
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
    parse_tokens(
        tokens,
        "\n\nmodule com.example.foo application \"bar\"\n\nimport git \"https://github.com/example/baz\" \"main\"\n");
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

}; // TEST_SUITE(tokenizer_suite)
