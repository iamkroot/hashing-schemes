#ifndef BUCKET_HPP
#define BUCKET_HPP

#include "common.h"
#include "DiskManager.hpp"

#include <cereal/archives/binary.hpp>

template<typename K, typename V>
class Bucket {
public:
    uint64_t page_id;
    uint64_t local_depth{0};
    DiskManager* dm;

    explicit Bucket(DiskManager* dm) : dm(dm) {
        page_id = dm->new_page();
    }

    explicit Bucket(DiskManager* dm, uint64_t pageId, uint64_t localDepth = 0) : dm(dm), page_id(pageId),
                                                                                 local_depth(localDepth) {}

    bool find(const K &key, V &value) {
        char page_data[PAGE_SIZE];
        dm->read_page(page_id, page_data);

    }

    bool insert(const K &key, const V &value) {
        return false;
    }

    bool remove(const K &key) {
        return false;
    }

    bool is_full() {
        return false;
    }

    bool is_empty() {
        return false;
    }

    bool clear() {
        return false;
    }
};


#endif //BUCKET_HPP
