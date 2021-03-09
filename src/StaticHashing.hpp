#ifndef STATICHASHING_HPP
#define STATICHASHING_HPP

#include "DiskManager.hpp"
#include "HashingScheme.hpp"
#include "Bucket.hpp"

template<typename K, typename V>
class StaticHashing : HashingScheme<K, V> {
public:
    bool insert(const K &key, const V &value) override {
      std::size_t hashed_key = std::hash(K);
      Bucket.insert(hashed_key(K),value);
      return false;
    }

    bool get(const K &key, V* value) override {
      if (find(key,value)) {
      return true;
    }
    return false;
    }

    bool remove(const K &key) override {

        return false;
    }



private:
    uint64_t num_slots;
};

#endif //STATICHASHING_HPP
