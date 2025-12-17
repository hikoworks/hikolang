
#include "repository.hpp"
#include "test_utilities/paths.hpp"
#include "utility/path.hpp"
#include <hikotest/hikotest.hpp>
#include <ranges>
#include <filesystem>

TEST_SUITE(repository_suite) 
{

TEST_CASE(single_repository_scan) 
{
    auto test_data_path = test::test_data_path();
    auto repository_path = std::filesystem::canonical(test_data_path / "single_repository_scan");
    auto repository = hk::repository{repository_path};
    repository.scan_prologues(hk::datum_namespace{});

    auto nodes = std::ranges::to<std::vector>(repository.remote_repositories(hk::datum_namespace{}));
    REQUIRE(nodes.size() == 1);
    REQUIRE(nodes[0]->url.url() == "https://github.com/hikoworks/hikolang-test-a.git");
    REQUIRE(nodes[0]->url.rev() == "main");
}

TEST_CASE(recursive_repository_scan) 
{
    auto source_path = std::filesystem::canonical(test::test_data_path() / "recursive_repository_scan");
    auto repository_path = hk::scoped_temporary_directory("recursive_repository_scan");
    std::filesystem::copy(source_path, repository_path);

    auto repository = hk::repository{repository_path};
    repository.recursive_scan_prologues(hk::datum_namespace{}, hk::repository_flags{});

    REQUIRE(repository.child_repositories().size() == 2);
    REQUIRE(repository.child_repositories()[0]->remote.url() == "https://github.com/hikoworks/hikolang-test-a.git");
    REQUIRE(repository.child_repositories()[1]->remote.url() == "https://github.com/hikoworks/hikolang-test-b.git");

    //REQUIRE(repository.anchors().size() == 2);
}

//TEST_CASE(parse_repository)
//{
//    auto source_path = std::filesystem::canonical(test::test_data_path() / "return42");
//    auto repository_path = hk::scoped_temporary_directory("parse_repository");
//    std::filesystem::copy(source_path, repository_path);
//
//    auto repository = hk::repository{repository_path};
//    repository.recursive_scan_prologues(hk::repository_flags{});
//    repository.parse_files();
//
//}

};
