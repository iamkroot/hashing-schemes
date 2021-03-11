#ifndef STATICHASHING_HPP
#define STATICHASHING_HPP

#include "DiskManager.hpp"
#include "HashingScheme.hpp"
#include "Bucket.hpp"

template<typename K, typename V>
class StaticHashing : HashingScheme<K, V> {
public:
    bool insert(const K &key, const V &value) override {
      std::size_t hashed_key = std::hash(key);
      return Bucket.insert(hashed_key%num_slots,value)
      return false;
    }

    bool get(const K &key, V* value) override {
      std::size_t hashed_key = std::hash(key);
      if (find(size_t%num_slots,value)) {
      return true;
    }
    return false;
    }

    bool remove(const K &key) override {
      std::size_t hashed_key = std::hash(key);
      return Bucket.remove(hashed_key%num_slots,value);
      return false;
    }



private:
    uint64_t num_slots;
};

#endif //STATICHASHING_HPP
