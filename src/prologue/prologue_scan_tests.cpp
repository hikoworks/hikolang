#include "prologue_scan.hpp"
#include "utility/unit_test.hpp"
#include <hikotest/hikotest.hpp>
#include <print>

TEST_SUITE(prologue_scan_suite) 
{

TEST_CASE(prologue_scan_repository_test) 
{
    auto test_data_path = hk::test_data_path();
    auto repository_path = test_data_path / "prologue_scan" / "root";
    auto modules = hk::prologue_scan_repository(repository_path);
    REQUIRE(modules.size() == 1);

    auto &module = modules[0];
    REQUIRE(module->errors.empty());
    REQUIRE(module->declaration != nullptr);
    REQUIRE(module->declaration->name == "nl.hikolang.prologue_scan.main");
}

};
