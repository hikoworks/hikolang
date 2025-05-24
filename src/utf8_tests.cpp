
#include "utf8.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(utf8_suite) 
{

TEST_CASE(utf8_decode_valid) 
{
    auto const utf8_str = std::string{"Hello, 世界!"};
    auto utf32_str = std::u32string{};
    auto it = utf8_str.begin();
    auto const end = utf8_str.end();

    while (it != end) {
        auto const code_point = hl::decode_utf8_code_point(it, end);
        utf32_str.push_back(code_point);

        REQUIRE(code_point <= 0x10ffff);
    }

    REQUIRE(utf32_str == U"Hello, 世界!");
}

};
