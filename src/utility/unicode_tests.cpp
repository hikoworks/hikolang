
#include "unicode.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(unicode_suite) 
{

TEST_CASE(unicode_decode_valid) 
{
    auto const utf8_str = std::string{"Hello, 世界!"};
    auto utf32_str = std::u32string{};
    auto ptr = utf8_str.data();
    auto const end = ptr + utf8_str.size();

    while (ptr != end) {
        auto const code_point = hl::decode_utf8_code_point(ptr, end);
        utf32_str.push_back(code_point);

        REQUIRE(code_point <= 0x10ffff);
    }

    REQUIRE(utf32_str == U"Hello, 世界!");
}


};
