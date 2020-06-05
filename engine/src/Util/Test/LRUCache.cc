#include <cstring>

#include "Test/catch.h"
#include "Util/LRUCache.h"

static int s_counter = 0;

template<typename T>
int get(const T& /* unused */)
{
    return ++s_counter;
}

TEST_CASE("LRUCache", "[Util][Util.LRUCache]")
{
    SECTION("constructor & destructor")
    {
        LRUCache<double, int> int_cache(get<double>, 10);
        LRUCache<std::string, int> str_cache(get<std::string>, 10);
        LRUCache<int*, int> int_ptr_cache(get<int*>, 10);
    }

    SECTION("get()")
    {
        LRUCache<int, int> cache(get<int>, 10);
        REQUIRE(cache.size() == 0);
        REQUIRE(cache.get(10) == 1);
        REQUIRE(cache.size() == 1);
        REQUIRE(cache.get(11) == 2);
        REQUIRE(cache.size() == 2);
        REQUIRE(cache.get(12) == 3);
        REQUIRE(cache.size() == 3);
        REQUIRE(cache.get(10) == 1);
        REQUIRE(cache.get(11) == 2);
        REQUIRE(cache.get(12) == 3);
        REQUIRE(cache.size() == 3);
    }

    SECTION("eviction w/ callback")
    {
        std::vector<int> evictions;

        LRUCache<int, int> cache(
            get<int>, [&](int&, int& value) { evictions.push_back(value); }, 3);

        REQUIRE(cache.get(10) == 4);
        REQUIRE(cache.get(11) == 5);
        REQUIRE(cache.get(12) == 6);
        REQUIRE(cache.size() == 3);

        // force a miss & eviction
        REQUIRE(cache.get(13) == 7);
        REQUIRE(cache.size() == 3);

        // non-evicted keys shouldn't change
        REQUIRE(cache.get(11) == 5);
        REQUIRE(cache.get(12) == 6);

        // evicted key should be refetched (causing another eviction)
        REQUIRE(cache.get(10) == 8);

        REQUIRE(cache.size() == 3);
        REQUIRE(evictions == std::vector<int>{ 4, 7 });
    }

    SECTION("eviction w/o callback")
    {
        LRUCache<int, int> cache(get<int>, 3);

        REQUIRE(cache.get(10) == 9);
        REQUIRE(cache.get(11) == 10);
        REQUIRE(cache.get(12) == 11);
        REQUIRE(cache.size() == 3);

        REQUIRE(cache.get(13) == 12);
        REQUIRE(cache.size() == 3);

        REQUIRE(cache.get(11) == 10);
        REQUIRE(cache.get(12) == 11);

        REQUIRE(cache.get(10) == 13);

        REQUIRE(cache.size() == 3);
    }

    SECTION("lettuce example")
    {
        LRUCache<std::string, int> counts(get<std::string>, 4);

        counts.get("hello");
        counts.get("lettuce");
        counts.get("hey");
        counts.get("what");
        counts.get("is");
        counts.get("beadly");
    }
}
