#include <hikotest/hikotest.hpp>
#include "vector_map.hpp"

using map_type = hk::vector_map<int, int>;

TEST_SUITE(vector_map_suite)
{

TEST_CASE(empty_map)
{
    map_type m;

    REQUIRE(m.empty());
    REQUIRE(m.size() == 0);
    REQUIRE(not m.contains(1));
}

TEST_CASE(single_insert)
{
    map_type m;

    auto [it, inserted] = m.try_emplace(1, 10);

    REQUIRE(inserted);
    REQUIRE(it != m.end());
    REQUIRE(it->key == 1);
    REQUIRE(it->value == 10);
    REQUIRE(m.size() == 1);
    REQUIRE(m.contains(1));
}

TEST_CASE(duplicate_insert)
{
    map_type m;

    auto [it1, inserted1] = m.try_emplace(1, 10);
    REQUIRE(inserted1);

    auto [it2, inserted2] = m.try_emplace(1, 20);
    REQUIRE(not inserted2);

    REQUIRE(it1 == it2);
    REQUIRE(it2->value == 10);
    REQUIRE(m.size() == 1);
}

TEST_CASE(multiple_unsorted_inserts)
{
    map_type m;

    // Insert in descending order to stress unsorted behavior
    for (int i = 9; i >= 0; --i) {
        auto [it, inserted] = m.try_emplace(i, i * 10);
        REQUIRE(inserted);
    }

    REQUIRE(m.size() == 10);

    for (int i = 0; i < 10; ++i) {
        REQUIRE(m.contains(i));
    }
}

TEST_CASE(iteration_is_sorted)
{
    map_type m;

    m.try_emplace(3, 30);
    m.try_emplace(1, 10);
    m.try_emplace(4, 40);
    m.try_emplace(2, 20);
    m.consolidate();

    int expected_key = 1;
    for (auto const& item : m) {
        REQUIRE(item.key == expected_key);
        REQUIRE(item.value == expected_key * 10);
        ++expected_key;
    }

    REQUIRE(expected_key == 5);
}

TEST_CASE(find_existing_and_missing)
{
    map_type m;

    m.try_emplace(1, 10);
    m.try_emplace(3, 30);
    m.try_emplace(5, 50);

    auto it = m.find(3);
    REQUIRE(it != m.end());
    REQUIRE(it->value == 30);

    auto it_missing = m.find(2);
    REQUIRE(it_missing == m.end());
}

TEST_CASE(erase_existing)
{
    map_type m;

    m.try_emplace(1, 10);
    m.try_emplace(2, 20);
    m.try_emplace(3, 30);

    REQUIRE(m.erase(2) == 1);
    REQUIRE(not m.contains(2));
    REQUIRE(m.size() == 2);

    REQUIRE(m.contains(1));
    REQUIRE(m.contains(3));
}

TEST_CASE(erase_missing)
{
    map_type m;

    m.try_emplace(1, 10);
    m.try_emplace(2, 20);

    REQUIRE(m.erase(3) == 0);
    REQUIRE(m.size() == 2);
}

TEST_CASE(clear_map)
{
    map_type m;

    for (int i = 0; i < 10; ++i) {
        m.try_emplace(i, i);
    }

    REQUIRE(not m.empty());

    m.clear();

    REQUIRE(m.empty());
    REQUIRE(m.size() == 0);
    REQUIRE(not m.contains(0));
}

TEST_CASE(stress_unsorted_then_find)
{
    map_type m;

    // Fill enough elements to force multiple consolidations
    for (int i = 0; i < 100; ++i) {
        m.try_emplace(i, i * 2);
    }

    REQUIRE(m.size() == 100);

    for (int i = 0; i < 100; ++i) {
        auto it = m.find(i);
        REQUIRE(it != m.end());
        REQUIRE(it->value == i * 2);
    }
}

};
