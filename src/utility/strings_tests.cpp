
#include "utility/strings.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(strings_suite) 
{
    TEST_CASE(split_zero_test) {
        auto result = hk::split("", ':');
        REQUIRE(result.size() == 0);
    }

    TEST_CASE(split_one_test) {
        auto result = hk::split("one", ':');
        REQUIRE(result.size() == 1);
        REQUIRE(result[0] == "one");
    }

    TEST_CASE(split_two_test) {
        auto result = hk::split("one:two", ':');
        REQUIRE(result.size() == 2);
        REQUIRE(result[0] == "one");
        REQUIRE(result[1] == "two");
    }

    TEST_CASE(split_two_empty_test) {
        auto result = hk::split(":", ':');
        REQUIRE(result.size() == 2);
        REQUIRE(result[0] == "");
        REQUIRE(result[1] == "");
    }
};
