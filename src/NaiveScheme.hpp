#ifndef NAIVESCHEME_HPP
#define NAIVESCHEME_HPP

#include <list>

#include "DiskManager.hpp"
#include "HashingScheme.hpp"
#include "Bucket.hpp"

/**
 * @brief Represents the naive file organization scheme, where every new entry is inserted at the end
 */
template<typename K, typename V>
class NaiveScheme : public HashingScheme<K, V> {
    DiskManager* dm;
    std::list<Bucket<K, V>> buckets;
public:
    NaiveScheme(DiskManager* dm) : dm(dm) {}

    bool insert(const K &key, const V &value) override {
        if (buckets.empty() || buckets.back().is_full()) {
            buckets.emplace_back(dm);
        }
        return buckets.back().insert(key, value);
    }

    bool get(const K &key, V* value) override {
        for (auto &bucket: buckets) {
            if (bucket.find(key, value)) {
                return true;
            }
        }
        return false;
    }

    bool remove(const K &key) override {
        for (auto &bucket: buckets) {
            if (bucket.remove(key)) {
                return true;
            }
        }
        return false;
    }

};

#endif //NAIVESCHEME_HPP
