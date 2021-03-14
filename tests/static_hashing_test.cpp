#include "doctest.h"
#include "common.hpp"
#include "StaticHashing.hpp"

TEST_SUITE("StaticHashing") {
    TEST_CASE_FIXTURE(DiskManagerFixture, "Insert") {
        StaticHashing<int, int> static_hash(10, &dm);
        bool ret = static_hash.insert(4, 2);
        REQUIRE(ret == true);
    }

    TEST_CASE_FIXTURE(DiskManagerFixture, "Get") {
        StaticHashing<int, int> static_hash(10, &dm);
        static_hash.insert(4, 2);
        int v;
        SUBCASE("Get exists") {
            bool ret = static_hash.get(4, &v);
            REQUIRE(ret == true);
            REQUIRE(v == 2);

        }
        SUBCASE("Get missing key") {
            bool ret = static_hash.get(2, &v);
            REQUIRE(ret == false);

        }
    }
}