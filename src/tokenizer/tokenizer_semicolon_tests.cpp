
#include "tokenizer.hpp"
#include "utility/read_file.hpp"
#include "utility/lazy_vector.hpp"
#include "test_utilities/paths.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(tokenizer_semicolon_suite) {
#define parse_tokens(output, str) \
    auto text = std::string{str}; \
    auto lines = hk::line_table{}; \
    lines.add(text.data(), 0, "<text>"); \
    auto token_generator = hk::tokenize(text.data(), lines); \
    auto output = hk::lazy_vector \
    { \
        token_generator.cbegin(), token_generator.cend() \
    }

TEST_CASE(id_end)
{
    parse_tokens(tokens, "a");
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(id_lf)
{
    parse_tokens(tokens, "a\n");
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(id_sc_lf)
{
    parse_tokens(tokens, "a;\n");
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(id_lf_lf)
{
    parse_tokens(tokens, "a\n\n");
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(id_lf_id_lf)
{
    parse_tokens(tokens, "a\nb\n");
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == "b");
    REQUIRE(tokens[3] == ';');
    REQUIRE(tokens[4] == '\0');
}

TEST_CASE(id_lc)
{
    parse_tokens(tokens, "a//foo");
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(id_lc_lf)
{
    parse_tokens(tokens, "a//foo\n");
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(id_bc)
{
    parse_tokens(tokens, "a/*foo*/");
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(id_bc_lf)
{
    parse_tokens(tokens, "a/*foo*/\n");
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(id_ld)
{
    parse_tokens(tokens, "a///foo");
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(id_ld_lf)
{
    parse_tokens(tokens, "a///foo\n");
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(id_bd)
{
    parse_tokens(tokens, "a/**foo*/");
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(id_bd_lf)
{
    parse_tokens(tokens, "a/**foo*/\n");
    REQUIRE(tokens[0] == "a");
    REQUIRE(tokens[1] == ';');
    REQUIRE(tokens[2] == '\0');
}

TEST_CASE(obrace_id_cbrace)
{
    parse_tokens(tokens, "{a}");
    REQUIRE(tokens[0] == '{');
    REQUIRE(tokens[1] == "a");
    REQUIRE(tokens[2] == ';');
    REQUIRE(tokens[3] == '}');
    REQUIRE(tokens[4] == ';');
    REQUIRE(tokens[5] == '\0');
}

TEST_CASE(obrace_id_lf_cbrace)
{
    parse_tokens(tokens, "{a\n}");
    REQUIRE(tokens[0] == '{');
    REQUIRE(tokens[1] == "a");
    REQUIRE(tokens[2] == ';');
    REQUIRE(tokens[3] == '}');
    REQUIRE(tokens[4] == ';');
    REQUIRE(tokens[5] == '\0');
}

TEST_CASE(obrace_id_lf_cbrace_lf)
{
    parse_tokens(tokens, "{a\n}\n");
    REQUIRE(tokens[0] == '{');
    REQUIRE(tokens[1] == "a");
    REQUIRE(tokens[2] == ';');
    REQUIRE(tokens[3] == '}');
    REQUIRE(tokens[4] == ';');
    REQUIRE(tokens[5] == '\0');
}

TEST_CASE(opar_id_cpar)
{
    parse_tokens(tokens, "(a)");
    REQUIRE(tokens[0] == '(');
    REQUIRE(tokens[1] == "a");
    REQUIRE(tokens[2] == ')');
    REQUIRE(tokens[3] == ';');
    REQUIRE(tokens[4] == '\0');
}

TEST_CASE(opar_id_lf_cpar)
{
    parse_tokens(tokens, "(a\n)");
    REQUIRE(tokens[0] == '(');
    REQUIRE(tokens[1] == "a");
    REQUIRE(tokens[2] == ')');
    REQUIRE(tokens[3] == ';');
    REQUIRE(tokens[4] == '\0');
}

TEST_CASE(opar_id_lf_cpar_lf)
{
    parse_tokens(tokens, "(a\n)\n");
    REQUIRE(tokens[0] == '(');
    REQUIRE(tokens[1] == "a");
    REQUIRE(tokens[2] == ')');
    REQUIRE(tokens[3] == ';');
    REQUIRE(tokens[4] == '\0');
}

}; // TEST_SUITE(tokenizer_semicolon_suite)
