#ifndef BUCKET_HPP
#define BUCKET_HPP

#include "common.h"
#include "DiskManager.hpp"
#include <unordered_map>
#include <cereal/types/string.hpp>
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

    bool contains(const K &key) {
        std::unordered_map<K, V> map = read_page();
        return map.contains(key);
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
        int peek_size = 16;  // number of bytes to peek
        char* data;
        size_t size;  // bytes taken by the serialized form of map (actual data size)
        size_t size_len;  // number of bytes used by 'size' variable serialization itself
        char next_char;  // the character immediately to the right of the last digit of 'size'
        do {
            data = new char[peek_size];
            char* end = nullptr;  // points to the character just after the 'size'
            dm->peek_page(page_id, peek_size, data);
            size = strtoull(data, &end, 10);
            next_char = *end;
            peek_size *= 2;
            size_len = end - data;
            delete[] data;
            // according to format, the size is followed by a space. In case we didn't hit a space, this means the number of
            // bytes taken up by 'size' itself, is more than what we peeked. So we retry after doubling the peek_size
        } while (next_char != ' ');
        // end points to the end of the size variable
        const uint8_t extra = sizeof(K) + sizeof(V); // space used by one entry in the bucket
        return size_len + size + extra >= PAGE_SIZE;
    }

    bool is_empty() {
        const auto map = read_page();
        return map.empty();
    }

    void clear() {
        write_page({});
    }

    std::unordered_map<K, V> read_page() {
        char page_data[PAGE_SIZE];
        dm->read_page(page_id, page_data);
        std::istringstream ss(std::stringstream::in | std::stringstream::binary);
        char* end;
        std::streamsize size = strtol(page_data, &end, 10);
        ss.rdbuf()->pubsetbuf(end + 1, size);
        cereal::BinaryInputArchive archive(ss);
        std::unordered_map<K, V> map;
        archive(map);
        return map;
    }

private:
    void write_page(std::unordered_map<K, V> &&map) {
        char page_data[PAGE_SIZE];
        std::ostringstream ss(std::stringstream::out | std::stringstream::binary);
        cereal::BinaryOutputArchive archive(ss);

        archive(map);
        auto s = ss.str();
        auto size = static_cast<std::streamsize>(s.size());
        int start = sprintf(page_data, "%" PRId64 " ", size);
        memcpy(page_data + start, s.c_str(), size);
        dm->write_page(page_id, page_data);
    }
};


#endif //BUCKET_HPP
