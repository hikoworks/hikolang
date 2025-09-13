#include "repository.hpp"
#include "utility/unit_test.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(repository_suite) 
{

TEST_CASE(single_repository_scan) 
{
    auto test_data_path = hk::test_data_path();
    auto repository_path = test_data_path / "prologue_scan" / "root";
    auto repository = hk::repository{repository_path};
    repository.scan_prologues(false);

    REQUIRE()
}


};
