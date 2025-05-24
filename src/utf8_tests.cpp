
#include "utf8.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(utf8_suite) 
{

TEST_CASE(utf8_decode_valid) 
{
    auto utf8_str = std::string{"Hello, 世界!"};
    auto utf32_str = std::u32string{};
    auto it = utf8_str.begin();
    auto end = utf8_str.end();

    char32_t code_point;
    while (it != end) {
        code_point = hl::decode_utf8_code_point(it, end);
        utf32_str.push_back(code_point);

        REQUIRE(code_point != U'\ufffd');
    }

    REQUIRE(utf32_str == U"Hello, 世界!");
}

};
