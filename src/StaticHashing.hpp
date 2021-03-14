#ifndef STATICHASHING_HPP
#define STATICHASHING_HPP

#include <functional>
#include <list>
#include <vector>

#include "Bucket.hpp"
#include "DiskManager.hpp"
#include "HashingScheme.hpp"

template<typename K, typename V>
class StaticHashing : HashingScheme<K, V> {
    using HashFn = std::function<uint64_t(K)>;  // hash function type
private:
    uint64_t num_slots;
    std::vector<std::list<Bucket<K, V>>> slots;
    DiskManager* dm;
    HashFn hash_fn;

    /**
     * @brief Get a reference to the entire bucket chain for a given key
     * @param key Key to be hashed
     * @return List of buckets which might contain key entry
     */
    std::list<Bucket<K, V>> &get_bucket_chain(const K &key) {
        auto hashed = hash_fn(key);
        size_t slot = hashed % num_slots;
        return slots[slot];
    }

public:
    explicit StaticHashing(uint64_t numSlots, DiskManager* dm, HashFn hash_fn = std::hash<K>{}) : num_slots(numSlots),
                                                                                                  slots(numSlots),
                                                                                                  dm(dm),
                                                                                                  hash_fn(hash_fn) {}


    bool insert(const K &key, const V &value) override {
        auto &buckets = get_bucket_chain(key);

        // check if it is already present
        for (auto &bucket: buckets) {
            if (bucket.contains(key))
                return false;  // already exists
        }

        // check if no buckets or if last bucket full
        if (buckets.empty() || buckets.back().is_full()) {
            // add a new bucket
            buckets.emplace_back(dm);
        }

        return buckets.back().insert(key, value);
    }

    bool get(const K &key, V* value) override {
        auto &buckets = get_bucket_chain(key);
        for (auto &bucket: buckets) {
            if (bucket.find(key, value))
                return true;
        }
        return false;
    }

    bool remove(const K &key) override {
        return false;
    }
};

#endif //STATICHASHING_HPP
