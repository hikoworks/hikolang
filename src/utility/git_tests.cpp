
#include "git.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(git_suite) 
{
    TEST_CASE(git_list) {
        auto optional_list = hk::git_list("https://github.com/hikogui/hikogui.git");
        REQUIRE(static_cast<bool>(optional_list));
        auto list = std::move(optional_list).value();
        REQUIRE(not list.empty());
        auto it = list.find("main");
        REQUIRE(it != list.end());
    }
};
