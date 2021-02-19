#ifndef STATICHASHING_HPP
#define STATICHASHING_HPP

#include "DiskManager.hpp"
#include "HashingScheme.hpp"

template<typename K, typename V>
class StaticHashing : HashingScheme<K, V> {
public:
    bool insert(const K &key, const V &value) override {
        return false;
    }

    bool get(const K &key, V* value) override {
        return false;
    }

    bool remove(const K &key) override {
        return false;
    }

private:
    uint64_t num_slots;
};

#endif //STATICHASHING_HPP
