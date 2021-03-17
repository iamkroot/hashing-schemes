#include "doctest.h"
#include "common.hpp"
#include "NaiveScheme.hpp"
#include "StaticHashing.hpp"
#include "Stopwatch.hpp"

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

    constexpr int num_entries = 50000;
    constexpr int num_lookups = 100000;

    auto gen_lookups() noexcept {
        std::array<int, num_lookups> nums = {};
        for (int &num : nums) {
            num = std::rand() % num_entries;
        }
        return nums;
    }

    std::array<int, num_lookups> lookups = gen_lookups();

    TEST_CASE_FIXTURE(DiskManagerFixture, "Perf") {
        HashingScheme<int, int>* scheme;  // base class, will be assigned to from each subcase
        SUBCASE("Naive") {
            scheme = new NaiveScheme<int, int>(&dm);
        }
        SUBCASE("Static") {
            for (uint64_t num_slots: {5, 10, 20, 50, 100, 200, 500, 1000}) {
                SUBCASE((std::to_string(num_slots) + " slots").c_str()) {
                    scheme = new StaticHashing<int, int>{num_slots, &dm};
                }
            }
        }
        dm.reset_stats();
        Stopwatch sw;
        for (int i = 0; i < num_entries; ++i) {
            scheme->insert(i, i);
        }
        auto insertion_time = sw.stop();
        MESSAGE("Insertion Time: ", insertion_time, "us");
        MESSAGE("Pages Used: ", dm.last_used_page + 1);
        MESSAGE("DM Reads: ", dm.num_reads);
        MESSAGE("DM Peeks: ", dm.num_peeks);
        MESSAGE("DM Writes: ", dm.num_writes);
        MESSAGE("DM Insertion Page Accesses: ", dm.num_reads + dm.num_writes);
        dm.reset_stats();
        int v;
        for (int &lookup : lookups) {
            scheme->get(lookup, &v);
        }
        auto lookup_time = sw.stop();
        MESSAGE("Lookup Time: ", lookup_time, "us");
        MESSAGE("DM Reads: ", dm.num_reads);
        MESSAGE("DM Peeks: ", dm.num_peeks);
        MESSAGE("DM Lookup Page Accesses: ", dm.num_reads);
        delete scheme;
    }
}