
#include "defer.hpp"
#include <hikotest/hikotest.hpp>

TEST_SUITE(defer_suite) 
{
    TEST_CASE(simple) {
        int i = 1;

        {
            auto const d1 = hk::defer([&]{
                i += 8;
            });

            auto const d2 = hk::defer([&]{
                i *= 3;
            });
        }

        // If executed in the wrong order i == 27.
        REQUIRE(i == 11);
    }
};
