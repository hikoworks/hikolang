#include <hikotest/hikotest.hpp>
#include "vector_set.hpp"

using set_type = hk::vector_set<int>;

// This requires [[no_unique_address]] to work, which it should be on clang.
static_assert(sizeof(set_type::item_type) == sizeof(set_type::key_type));

TEST_SUITE(vector_set_suite)
{

TEST_CASE(empty_set)
{
    set_type m;

    REQUIRE(m.empty());
    REQUIRE(m.size() == 0);
    REQUIRE(not m.contains(1));
}

TEST_CASE(single_insert)
{
    set_type m;

    auto [it, inserted] = m.try_emplace(1);

    REQUIRE(inserted);
    REQUIRE(it != m.end());
    REQUIRE(it->key == 1);
    REQUIRE(m.size() == 1);
    REQUIRE(m.contains(1));
}

TEST_CASE(duplicate_insert)
{
    set_type m;

    auto [it1, inserted1] = m.try_emplace(1);
    REQUIRE(inserted1);

    auto [it2, inserted2] = m.try_emplace(1);
    REQUIRE(not inserted2);

    REQUIRE(it1 == it2);
    REQUIRE(m.size() == 1);
}

TEST_CASE(multiple_unsorted_inserts)
{
    set_type m;

    // Insert in descending order to stress unsorted behavior
    for (int i = 9; i >= 0; --i) {
        auto [it, inserted] = m.try_emplace(i);
        REQUIRE(inserted);
    }

    REQUIRE(m.size() == 10);

    for (int i = 0; i < 10; ++i) {
        REQUIRE(m.contains(i));
    }
}

TEST_CASE(iteration_is_sorted)
{
    set_type m;

    m.try_emplace(3);
    m.try_emplace(1);
    m.try_emplace(4);
    m.try_emplace(2);
    m.consolidate();

    int expected_key = 1;
    for (auto const& item : m) {
        REQUIRE(item.key == expected_key);
        ++expected_key;
    }

    REQUIRE(expected_key == 5);
}

TEST_CASE(find_existing_and_missing)
{
    set_type m;

    m.try_emplace(1);
    m.try_emplace(3);
    m.try_emplace(5);

    auto it = m.find(3);
    REQUIRE(it != m.end());

    auto it_missing = m.find(2);
    REQUIRE(it_missing == m.end());
}

TEST_CASE(erase_existing)
{
    set_type m;

    m.try_emplace(1);
    m.try_emplace(2);
    m.try_emplace(3);

    REQUIRE(m.erase(2) == 1);
    REQUIRE(not m.contains(2));
    REQUIRE(m.size() == 2);

    REQUIRE(m.contains(1));
    REQUIRE(m.contains(3));
}

TEST_CASE(erase_missing)
{
    set_type m;

    m.try_emplace(1);
    m.try_emplace(2);

    REQUIRE(m.erase(3) == 0);
    REQUIRE(m.size() == 2);
}

TEST_CASE(clear_set)
{
    set_type m;

    for (int i = 0; i < 10; ++i) {
        m.try_emplace(i);
    }

    REQUIRE(not m.empty());

    m.clear();

    REQUIRE(m.empty());
    REQUIRE(m.size() == 0);
    REQUIRE(not m.contains(0));
}

TEST_CASE(stress_unsorted_then_find)
{
    set_type m;

    // Fill enough elements to force multiple consolidations
    for (int i = 0; i < 100; ++i) {
        m.try_emplace(i);
    }

    REQUIRE(m.size() == 100);

    for (int i = 0; i < 100; ++i) {
        auto it = m.find(i);
        REQUIRE(it != m.end());
    }
}

};
