#include "utility/thread_pool.hpp"
#include <hikotest/hikotest.hpp>
#include <atomic>
#include <bitset>

TEST_SUITE(thread_pool_suite) {
TEST_CASE(simple_test)
{
    auto pool = hk::thread_pool(2);
    auto count = std::atomic<size_t>{0};
    auto futures = std::array<std::future<size_t>, 10>{};

    for (auto i = 0uz; i != futures.size(); ++i) {
        futures[i] = pool([&] {
            return count++;
        });
    }

    // Wait for completion of all jobs.
    for (auto i = 0uz; i != futures.size(); ++i) {
        futures[i].wait();
    }

    REQUIRE(count == futures.size());

    // Check if all the results of the jobs are accounted for.
    auto check = std::bitset<futures.size()>{};
    for (auto i = 0uz; i != futures.size(); ++i) {
        auto j = futures[i].get();
        assert(j < futures.size());
        check.set(j);
    }
    REQUIRE(check.all());
}
}; // TEST_SUITE(thread_pool_suite)
