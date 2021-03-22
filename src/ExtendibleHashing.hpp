#ifndef EXTENDIBLEHASHING_HPP
#define EXTENDIBLEHASHING_HPP

#include "common.h"
#include <functional>
#include "Bucket.hpp"
#include "HashingScheme.hpp"
#include "DiskManager.hpp"

template<typename K, typename V>
class ExtendibleHashing : public HashingScheme<K, V> {
    using HashFn = std::function<uint64_t(K)>;  // hash function type
    DiskManager* dm;
    HashFn hash_fn;

    uint32_t global_depth;
    uint32_t num_buckets;  // keep track of unique buckets till now
    std::vector<std::shared_ptr<Bucket<K, V>>> buckets;

    /**
     * Use global depth to find the appropriate bucket for the key
     */
    uint32_t get_bucket_idx(const K &key) const {
        // use the least significant bits of the hashed output as the index
        return hash_fn(key) & ((1 << global_depth) - 1);
    }

    /**
     * Grow directory by doubling its size
     */
    void grow() {
        buckets.reserve(1 << ++global_depth);
        std::copy(buckets.begin(), buckets.end(), std::back_inserter(buckets));
    }

public:
    /**
     * Initialize the structure with a single bucket
     */
    explicit ExtendibleHashing(DiskManager* dm, HashFn hash_fn = std::hash<K>{}) : dm(dm), hash_fn(hash_fn),
                                                                                   global_depth(0), num_buckets(1) {
        buckets.push_back(std::make_shared<Bucket<K, V>>(dm));
    }

    bool insert(const K &key, const V &value) override {
        const uint32_t bucket_idx = get_bucket_idx(key);
        auto bucket = buckets[bucket_idx];
        while (bucket->is_full()) {
            if (bucket->local_depth == global_depth) {
                grow();
            }
            // new sibling for bucket to be split
            auto sibling = std::make_shared<Bucket<K, V>>(dm);
            ++num_buckets;
            // the mask for the most significant bit that differs between the two buckets
            const uint32_t mask = 1 << bucket->local_depth;
            // update the local depths
            ++bucket->local_depth;
            sibling->local_depth = bucket->local_depth;

            // rehash the entries inside the original bucket
            for (auto &[key, value]:bucket->read_page()) {
                // ideally, half the entries would have the mask bit set
                if (get_bucket_idx(key) & mask) {
                    // move the entry to the new bucket
                    bucket->remove(key);
                    sibling->insert(key, value);
                }
            }

            // update the directory to point to the new bucket
            for (int i = 0; i < buckets.size(); ++i) {
                if (buckets[i] == bucket && i & mask)
                    buckets[i] = sibling;
            }
            bucket = buckets[get_bucket_idx(key)];
            // Edge case: All entries got rehashed into one bucket, need to split again, so loop back
        }

        return bucket->insert(key, value);
    }

    /**
     * @brief Get the value for the given key
     * @param[out] value Pointer to output value
     * @return True if the key was found successfully
     */
    bool get(const K &key, V* value) override {
        auto bucket = buckets[get_bucket_idx(key)];
        return bucket->find(key, value);
    }

    bool remove(const K &key) override {
        return false;
    }
};

#endif //EXTENDIBLEHASHING_HPP
