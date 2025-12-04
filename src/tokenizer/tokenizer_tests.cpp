
#include "tokenizer.hpp"
#include "utility/read_file.hpp"
#include "utility/lazy_vector.hpp"
#include "test_utilities/paths.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(tokenizer_suite) {
#define parse_tokens(output, str) \
    auto text = std::string{str}; \
    auto lines = hk::line_table{}; \
    lines.add(text.data(), 0, "<text>"); \
    auto token_generator = hk::tokenize(text.data(), lines); \
    auto output = hk::lazy_vector{token_generator.cbegin(), token_generator.cend()}

TEST_CASE(integer_12)
{
    parse_tokens(tokens, "12");
    REQUIRE(tokens[0].kind() == hk::token::integer_literal);
    REQUIRE(tokens[0].string_view() == "12");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(integer_space_12)
{
    parse_tokens(tokens, " 12");
    REQUIRE(tokens[0].kind() == hk::token::integer_literal);
    REQUIRE(tokens[0].string_view() == "12");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(integer_12_space)
{
    parse_tokens(tokens, "12 ");
    REQUIRE(tokens[0].kind() == hk::token::integer_literal);
    REQUIRE(tokens[0].string_view() == "12");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(integer_0b01)
{
    parse_tokens(tokens, "0b01");
    REQUIRE(tokens[0].kind() == hk::token::integer_literal);
    REQUIRE(tokens[0].string_view() == "0b01");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(integer_0B10)
{
    parse_tokens(tokens, "0B10");
    REQUIRE(tokens[0].kind() == hk::token::integer_literal);
    REQUIRE(tokens[0].string_view() == "0B10");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(integer_0x2a)
{
    parse_tokens(tokens, "0x2a");
    REQUIRE(tokens[0].kind() == hk::token::integer_literal);
    REQUIRE(tokens[0].string_view() == "0x2a");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(integer_0X2A)
{
    parse_tokens(tokens, "0X2A");
    REQUIRE(tokens[0].kind() == hk::token::integer_literal);
    REQUIRE(tokens[0].string_view() == "0X2A");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(short_file_file)
{
    auto test_data_path = test::test_data_path();
    auto path = test_data_path / "simple.hkm";
    auto text = hk::read_file(path, 8);
    REQUIRE(text.has_value());
    auto lines = hk::line_table{};
    lines.add(text->data(), 0, path.filename().string());

    auto token_generator = hk::tokenize(text->data(), lines);
    auto tokens = hk::lazy_vector{token_generator.cbegin(), token_generator.cend()};
    REQUIRE(tokens[0] == "module");
    REQUIRE(tokens[1] == "com");
    REQUIRE(tokens[2] == ".");
    REQUIRE(tokens[3] == "example");
    REQUIRE(tokens[4] == ".");
    REQUIRE(tokens[5] == "foo");
    REQUIRE(tokens[6] == "application");
    REQUIRE(tokens[7].string_view() == "bar");
    REQUIRE(tokens[8] == ';');
    REQUIRE(tokens[9] == "import");
    REQUIRE(tokens[10] == "git");
    REQUIRE(tokens[11].string_view() == "https://github.com/example/baz");
    REQUIRE(tokens[12].string_view() == "main");
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
    REQUIRE(tokens[7].string_view() == "bar");
    REQUIRE(tokens[8] == ';');
    REQUIRE(tokens[9] == "import");
    REQUIRE(tokens[10] == "git");
    REQUIRE(tokens[11].string_view() == "https://github.com/example/baz");
    REQUIRE(tokens[12].string_view() == "main");
    REQUIRE(tokens[13] == ';');
    REQUIRE(tokens[14] == '\0');
}

}; // TEST_SUITE(tokenizer_suite)
