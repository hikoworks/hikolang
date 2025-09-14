#include "repository.hpp"
#include "utility/unit_test.hpp"
#include <hikotest/hikotest.hpp>
#include <ranges>

TEST_SUITE(repository_suite) 
{

TEST_CASE(single_repository_scan) 
{
    auto test_data_path = hk::test_data_path();
    auto repository_path = std::filesystem::canonical(test_data_path / "prologue_scan" / "root");
    auto repository = hk::repository{repository_path};
    repository.scan_prologues(hk::repository_flags{});

    auto urls = std::ranges::to<std::vector>(repository.remote_repositories());
    REQUIRE(urls.size() == 1);
    REQUIRE(urls[0].first.url() == "https://github.com/hikogui/hikolang-tests-a.git");
    REQUIRE(urls[0].first.rev() == "main");
}


};
