
#include "utility/read_file.hpp"
#include "test_utilities/paths.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(read_file_suite) 
{
    TEST_CASE(read_file_not_exist) {
        auto const test_data_path = test::test_data_path();

        auto const optional_string = hk::read_file(test_data_path / "file_not_exist.txt");
        REQUIRE(not optional_string);
    }

    TEST_CASE(read_file) {
        auto test_data_path = test::test_data_path();

        auto const optional_string = hk::read_file(test_data_path / "read_file_test.txt");
        REQUIRE(optional_string.has_value());
        REQUIRE(optional_string->starts_with("Hello World"));
    }
};
