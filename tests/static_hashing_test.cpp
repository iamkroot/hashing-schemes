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

    TEST_CASE_FIXTURE(DiskManagerFixture, "NaiveTime") {
        NaiveScheme<int, int> naive(&dm);
        SUBCASE("Insertion") {
            Stopwatch sw;
            for (int i = 0; i < num_entries; ++i) {
                naive.insert(i, i);
            }
            auto insertion_time = sw.stop();
            MESSAGE("Naive Insertion Time: ", insertion_time, "us");
            MESSAGE("Pages Used: ", dm.last_used_page);
        }

        SUBCASE("Lookup") {
            for (int i = 0; i < num_entries; ++i) {
                naive.insert(i, i);
            }
            Stopwatch sw;
            int v;
            for (int &lookup : lookups) {
                naive.get(lookup, &v);
            }
            auto lookup_time = sw.stop();
            MESSAGE("Naive Lookup Time: ", lookup_time, "us");
        }
    }

    TEST_CASE_FIXTURE(DiskManagerFixture, "StaticTime") {
        auto run_tests = [&](int num_slots) {
            StaticHashing<int, int> static_scheme(num_slots, &dm);
            SUBCASE(("Insertion " + std::to_string(num_slots)).c_str()) {
                Stopwatch sw;
                for (int i = 0; i < num_entries; ++i) {
                    static_scheme.insert(i, i);
                }
                auto insertion_time = sw.stop();
                MESSAGE("Static Insertion Time: ", insertion_time, "us");
                MESSAGE("Pages Used: ", dm.last_used_page);
            }
            SUBCASE(("Lookup " + std::to_string(num_slots)).c_str()) {
                for (int i = 0; i < num_entries; ++i) {
                    static_scheme.insert(i, i);
                }
                Stopwatch sw;
                int v;
                for (int &lookup : lookups) {
                    static_scheme.get(lookup, &v);
                }
                auto lookup_time = sw.stop();
                MESSAGE("Static Lookup Time: ", lookup_time, "us");
            }
        };

        for (auto num_slots: {5, 10, 20, 50, 100, 200, 500, 1000}) {
            run_tests(num_slots);
        }
    }
}