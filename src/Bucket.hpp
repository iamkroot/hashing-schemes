#ifndef BUCKET_HPP
#define BUCKET_HPP

#include "common.h"
#include "DiskManager.hpp"
#include <unordered_map>
#include <cereal/types/unordered_map.hpp>
#include <cereal/archives/binary.hpp>
#include <cstring>
#include <cinttypes>

template<typename K, typename V>
class Bucket {
public:
    uint64_t page_id;
    uint64_t local_depth{0};
    DiskManager* dm;

    explicit Bucket(DiskManager* dm) : dm(dm) {
        page_id = dm->new_page();
        clear();
    }

    explicit Bucket(DiskManager* dm, uint64_t pageId, uint64_t localDepth = 0) : dm(dm), page_id(pageId),
                                                                                 local_depth(localDepth) {}

    bool find(const K &key, V* value) {
        std::unordered_map<K, V> map = read_page();
        auto it = map.find(key);
        if (it != map.end()) {
            *value = it->second;
            return true;
        }
        return false;
    }

    bool insert(const K &key, const V &value) {
        std::unordered_map<K, V> map = read_page();
        if (map.contains(key))
            return false;
        map[key] = value;
        write_page(std::move(map));
        return true;
    }

    bool remove(const K &key) {
        std::unordered_map<K, V> map = read_page();
        if (!map.contains(key))
            return false;
        map.erase(key);
        write_page(std::move(map));
        return true;
    }

    bool is_full() {
        char page_data[PAGE_SIZE];
        dm->read_page(page_id, page_data);
        std::istringstream ss(std::stringstream::in | std::stringstream::binary);
        char* end;
        uint64_t size = strtoull(page_data, &end, 10);
        if(size==PAGE_SIZE)
        return true;
        return false;
    }

    bool is_empty() {
        char page_data[PAGE_SIZE];
        dm->read_page(page_id, page_data);
        std::istringstream ss(std::stringstream::in | std::stringstream::binary);
        char* end;
        uint64_t size = strtoull(page_data, &end, 10);
        if(size==0)
        return true;
        return false;
    }

    void clear() {
        write_page({});
    }

private:
    std::unordered_map<K, V> read_page() {
        char page_data[PAGE_SIZE];
        dm->read_page(page_id, page_data);
        std::istringstream ss(std::stringstream::in | std::stringstream::binary);
        char* end;
        uint64_t size = strtoull(page_data, &end, 10);
        //char dd[size];
        char* dd = new char[size];
        memcpy(dd, end + 1, size);
        ss.rdbuf()->pubsetbuf(dd, size);
        cereal::BinaryInputArchive archive(ss);
        std::unordered_map<K, V> map;
        archive(map);
        delete [] dd;
        return map;
    }

    void write_page(std::unordered_map<K, V> &&map) {
        char page_data[PAGE_SIZE];
        std::ostringstream ss(std::stringstream::out | std::stringstream::binary);
        cereal::BinaryOutputArchive archive(ss);

        archive(map);
        auto s = ss.str();
        uint64_t size = s.size();
        int start = sprintf(page_data, "%" PRId64 " ", size);
        memcpy(page_data + start, s.c_str(), size);
        dm->write_page(page_id, page_data);
    }
};


#endif //BUCKET_HPP
