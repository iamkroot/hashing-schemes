#include "doctest.h"
#include "common.hpp"
#include "NaiveScheme.hpp"
#include "StaticHashing.hpp"
#include "ExtendibleHashing.hpp"
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

    TEST_CASE_FIXTURE(DiskManagerFixture, "Remove") {
        StaticHashing<int, int> static_hash(10, &dm);
        static_hash.insert(4, 2);
        SUBCASE("Remove exists") {
            bool ret = static_hash.remove(4);
            REQUIRE(ret == true);
        }
        SUBCASE("Remove missing key") {
            bool ret = static_hash.remove(2);
            REQUIRE(ret == false);
        }
    }

    constexpr int num_entries = 5000;
    constexpr int num_lookups = 10000;

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
        if constexpr(num_lookups <= 10000) {  // don't test naive for cases with lots of lookups
            SUBCASE("Naive") {
                scheme = new NaiveScheme<int, int>(&dm);
            }
        }
        SUBCASE("Static") {
            for (uint64_t num_slots: {5, 10, 20, 50, 100, 200, 500, 1000}) {
                SUBCASE((std::to_string(num_slots) + " slots").c_str()) {
                    scheme = new StaticHashing<int, int>{num_slots, &dm};
                }
            }
        }
        SUBCASE("Extendible") {
            scheme = new ExtendibleHashing<int, int>{&dm};
        }
        dm.reset_stats();
        Stopwatch sw;
        for (int i = 0; i < num_entries; ++i) {
            scheme->insert(i, i);
        }
        auto insertion_time = sw.stop();
        MESSAGE("Insertion Time: ", insertion_time, "us");
        MESSAGE("Pages Used: ", dm.last_used_page + 1);
        MESSAGE("DM Insertion Reads: ", dm.num_reads);
        MESSAGE("DM Insertion Peeks: ", dm.num_peeks);
        MESSAGE("DM Insertion Writes: ", dm.num_writes);
        MESSAGE("DM Insertion Page Accesses: ", dm.num_reads + dm.num_writes);
        dm.reset_stats();
        int v;
        for (int &lookup : lookups) {
            scheme->get(lookup, &v);
        }
        auto lookup_time = sw.stop();
        MESSAGE("Lookup Time: ", lookup_time, "us");
        MESSAGE("DM Lookup Reads: ", dm.num_reads);
        MESSAGE("DM Lookup Peeks: ", dm.num_peeks);
        MESSAGE("DM Lookup Page Accesses: ", dm.num_reads);
        delete scheme;
    }
}