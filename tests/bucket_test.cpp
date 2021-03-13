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
    TEST_CASE_FIXTURE(DiskManagerFixture, "Insert/Delete") {
        Bucket<int, int> b(&dm);
        b.insert(1, 2);
        b.insert(2, 3);
        b.remove(1);
        int v;
        REQUIRE(!b.find(1, &v));
        REQUIRE(!b.is_empty());
    }

}
