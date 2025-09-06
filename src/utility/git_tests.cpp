
#include "git.hpp"
#include "path.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(git_suite) 
{
    TEST_CASE(git_list) {
        auto optional_list = hk::git_list("https://github.com/hikogui/hikolang-test-a.git");
        REQUIRE(static_cast<bool>(optional_list));
        auto list = std::move(optional_list).value();
        REQUIRE(not list.empty());
        auto it = list.find("main");
        REQUIRE(it != list.end());
        it = list.find("v1.0.0");
        REQUIRE(it != list.end());
    }

    TEST_CASE(git_clone) {
        auto const tmp_dir = hk::scoped_temporary_directory("git_clone");

        auto const r = hk::git_clone("https://github.com/hikogui/hikolang-test-a.git", "main", tmp_dir.path());
        REQUIRE(r == hk::git_error::ok);
    }
};
