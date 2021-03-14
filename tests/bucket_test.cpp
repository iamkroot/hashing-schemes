#include "Bucket.hpp"
#include "doctest.h"
#include "common.hpp"

TEST_SUITE("Bucket") {
    TEST_CASE_FIXTURE(DiskManagerFixture, "Read/Write") {
        Bucket<int, int> b(&dm);
        b.insert(4, 5);
        int v;
        REQUIRE(b.find(4, &v));
        REQUIRE(v == 5);
    }

    TEST_CASE_FIXTURE(DiskManagerFixture, "Empty") {
        Bucket<int, int> b(&dm);
        REQUIRE(b.is_empty());
        b.insert(4, 5);
        REQUIRE(!b.is_empty());
    }

    TEST_CASE_FIXTURE(DiskManagerFixture, "Full") {
        Bucket<int, int> b(&dm);
        int i = 0;
        while (!b.is_full()) {
            b.insert(i, i * 2);
            ++i;
        }
        REQUIRE(i == 126);  // for page_size = 1KB, we can fit 126 (int, int) entries into the page
    }
}
