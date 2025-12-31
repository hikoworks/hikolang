
#include "utility/logic.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(logic_suite) {

/*
 *  | 0 | 1 | X | - |
 *  |---|---|---|---|
 *  | 1 | 0 | X | - |
 */
TEST_CASE(to_bool_test)
{
    REQUIRE(to_bool(hk::logic::F) == false);
    REQUIRE(to_bool(hk::logic::T) == true);
    REQUIRE(to_bool(hk::logic::X) == false);
    REQUIRE(to_bool(hk::logic::_) == true);
}

/*
 *  | 0 | 1 |
 *  |---|---|
 *  | 1 | 0 |
 */
TEST_CASE(to_logic_test)
{
    REQUIRE(hk::to_logic(false) == hk::logic::F);
    REQUIRE(hk::to_logic(true) == hk::logic::T);
}


/*
 *  |   || 0 | 1 | X | - |
 *  |---||---|---|---|---|
 *  | 0 || 0 | 0 | X | 0 |
 *  | 1 || 0 | 1 | X | - |
 *  | X || X | X | X | X |
 *  | - || 0 | - | X | - |
 */
TEST_CASE(and_test)
{
    REQUIRE((hk::logic::F & hk::logic::F) == hk::logic::F);
    REQUIRE((hk::logic::T & hk::logic::F) == hk::logic::F);
    REQUIRE((hk::logic::X & hk::logic::F) == hk::logic::X);
    REQUIRE((hk::logic::_ & hk::logic::F) == hk::logic::F);
    REQUIRE((hk::logic::F & hk::logic::T) == hk::logic::F);
    REQUIRE((hk::logic::T & hk::logic::T) == hk::logic::T);
    REQUIRE((hk::logic::X & hk::logic::T) == hk::logic::X);
    REQUIRE((hk::logic::_ & hk::logic::T) == hk::logic::_);
    REQUIRE((hk::logic::F & hk::logic::X) == hk::logic::X);
    REQUIRE((hk::logic::T & hk::logic::X) == hk::logic::X);
    REQUIRE((hk::logic::X & hk::logic::X) == hk::logic::X);
    REQUIRE((hk::logic::_ & hk::logic::X) == hk::logic::X);
    REQUIRE((hk::logic::F & hk::logic::_) == hk::logic::F);
    REQUIRE((hk::logic::T & hk::logic::_) == hk::logic::_);
    REQUIRE((hk::logic::X & hk::logic::_) == hk::logic::X);
    REQUIRE((hk::logic::_ & hk::logic::_) == hk::logic::_);
}

/*
 *  |   || 0 | 1 | x | - |
 *  |---||---|---|---|---|
 *  | 0 || 0 | 1 | X | - |
 *  | 1 || 1 | 1 | X | 1 |
 *  | X || X | X | X | X |
 *  | - || - | 1 | X | - |
 */
TEST_CASE(or_test)
{
    REQUIRE((hk::logic::F | hk::logic::F) == hk::logic::F);
    REQUIRE((hk::logic::T | hk::logic::F) == hk::logic::T);
    REQUIRE((hk::logic::X | hk::logic::F) == hk::logic::X);
    REQUIRE((hk::logic::_ | hk::logic::F) == hk::logic::_);
    REQUIRE((hk::logic::F | hk::logic::T) == hk::logic::T);
    REQUIRE((hk::logic::T | hk::logic::T) == hk::logic::T);
    REQUIRE((hk::logic::X | hk::logic::T) == hk::logic::X);
    REQUIRE((hk::logic::_ | hk::logic::T) == hk::logic::T);
    REQUIRE((hk::logic::F | hk::logic::X) == hk::logic::X);
    REQUIRE((hk::logic::T | hk::logic::X) == hk::logic::X);
    REQUIRE((hk::logic::X | hk::logic::X) == hk::logic::X);
    REQUIRE((hk::logic::_ | hk::logic::X) == hk::logic::X);
    REQUIRE((hk::logic::F | hk::logic::_) == hk::logic::_);
    REQUIRE((hk::logic::T | hk::logic::_) == hk::logic::T);
    REQUIRE((hk::logic::X | hk::logic::_) == hk::logic::X);
    REQUIRE((hk::logic::_ | hk::logic::_) == hk::logic::_);
}

/*
 *  |   || 0 | 1 | X | X |
 *  |---||---|---|---|---|
 *  | 0 || 0 | 1 | X | - |
 *  | 1 || 1 | 0 | X | - |
 *  | X || X | X | X | X |
 *  | - || - | - | X | - |
 */
TEST_CASE(xor_test)
{
    REQUIRE((hk::logic::F ^ hk::logic::F) == hk::logic::F);
    REQUIRE((hk::logic::T ^ hk::logic::F) == hk::logic::T);
    REQUIRE((hk::logic::X ^ hk::logic::F) == hk::logic::X);
    REQUIRE((hk::logic::_ ^ hk::logic::F) == hk::logic::_);
    REQUIRE((hk::logic::F ^ hk::logic::T) == hk::logic::T);
    REQUIRE((hk::logic::T ^ hk::logic::T) == hk::logic::F);
    REQUIRE((hk::logic::X ^ hk::logic::T) == hk::logic::X);
    REQUIRE((hk::logic::_ ^ hk::logic::T) == hk::logic::_);
    REQUIRE((hk::logic::F ^ hk::logic::X) == hk::logic::X);
    REQUIRE((hk::logic::T ^ hk::logic::X) == hk::logic::X);
    REQUIRE((hk::logic::X ^ hk::logic::X) == hk::logic::X);
    REQUIRE((hk::logic::_ ^ hk::logic::X) == hk::logic::X);
    REQUIRE((hk::logic::F ^ hk::logic::_) == hk::logic::_);
    REQUIRE((hk::logic::T ^ hk::logic::_) == hk::logic::_);
    REQUIRE((hk::logic::X ^ hk::logic::_) == hk::logic::X);
    REQUIRE((hk::logic::_ ^ hk::logic::_) == hk::logic::_);
}

/*
 *  | 0 | 1 | X | - |
 *  |---|---|---|---|
 *  | 1 | 0 | X | - |
 */
TEST_CASE(invert_test)
{
    REQUIRE(~hk::logic::F == hk::logic::T);
    REQUIRE(~hk::logic::T == hk::logic::F);
    REQUIRE(~hk::logic::X == hk::logic::X);
    REQUIRE(~hk::logic::_ == hk::logic::_);
}
}; // TEST_SUITE(logic_suite)
