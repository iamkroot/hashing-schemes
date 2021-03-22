#include "doctest.h"
#include "common.hpp"
#include "ExtendibleHashing.hpp"

TEST_SUITE("ExtendibleHashing") {
    TEST_CASE_FIXTURE(DiskManagerFixture, "Insert/Get") {
        ExtendibleHashing<int, int> eh(&dm, [](const int x) { return x + 1; });
        for (int i = 0; i < 1000; ++i) {
            eh.insert(i, i * 2);
        }
        for (int i = 0; i < 1000; ++i) {
            int v;
            eh.get(i, &v);
            REQUIRE(v == i * 2);
        }
    }

    TEST_CASE_FIXTURE(DiskManagerFixture, "Insert/Get2") {
        ExtendibleHashing<int, int> eh(&dm, [](const int x) { return x; });
        for (int i = 1000; i >= 0; --i) {
            eh.insert(i, i * 2);
        }
        for (int i = 0; i < 1000; ++i) {
            int v;
            eh.get(i, &v);
            REQUIRE(v == i * 2);
        }
    }
}