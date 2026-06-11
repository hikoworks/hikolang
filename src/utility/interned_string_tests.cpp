
#include "interned_string.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(interned_string_suite) 
{
    TEST_CASE(construct_test) {
        auto a = hk::interned_string("hello");
        auto b = hk::interned_string(std::string{"world"});
        auto c = hk::interned_string(std::string{"hello"});

        REQUIRE(a.size() == 5);
        REQUIRE(b.size() == 5);
        REQUIRE(c.size() == 5);
        REQUIRE(a.data() != b.data());
        REQUIRE(a.data() == c.data());

    }

    TEST_CASE(construct_empty_test) {
        auto a = hk::interned_string();
        auto b = hk::interned_string("");
        auto c = hk::interned_string(std::string{"world"});
        
        REQUIRE(a.data() != c.data());
        c.clear();

        REQUIRE(a.data() == b.data());
        REQUIRE(a.data() == c.data());
    }
};
