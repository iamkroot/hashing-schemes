#include "Bucket.hpp"
#include "DiskManager.hpp"
#include "doctest.h"

TEST_SUITE("Bucket") {
    TEST_CASE("temp") {
        auto dm = DiskManager("/tmp/tmp.db");
        Bucket<int, int> b(&dm);
    }
}
