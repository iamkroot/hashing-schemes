#ifndef DISKMANAGER_HPP
#define DISKMANAGER_HPP

#include <fstream>
#include <string>
#include <unordered_set>
#include <utility>
#include "common.h"

class DiskManager {
private:
    const std::string file_name;
    std::fstream db_file;
    const uint32_t page_size;
    IdT last_used_page;
    std::unordered_set<IdT> unused_pages;

public:
    uint64_t num_reads{};
    uint64_t num_peeks{};
    uint64_t num_writes{};

    explicit DiskManager(const std::string &file_name, uint32_t pageSize = PAGE_SIZE, IdT lastUsedPage = -1,
                         std::unordered_set<IdT> unusedPages = {})
            : file_name(file_name), page_size(pageSize), last_used_page(lastUsedPage),
              unused_pages(std::move(unusedPages)) {
        db_file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        if (!db_file.is_open()) {
            // create empty file
            db_file.open(file_name, std::ios::trunc | std::ios::out | std::ios::binary);
            db_file.close();
            // reopen with original mode
            db_file.open(file_name, std::ios::in | std::ios::out | std::ios::binary);
        }
    }

    IdT new_page() {
        if (unused_pages.empty()) {
            return ++last_used_page;
        } else {
            IdT page_id = *unused_pages.begin();
            unused_pages.erase(unused_pages.begin());
            return page_id;
        }
    }

    void remove_page(IdT page_id) {
        if (page_id == last_used_page) {
            --last_used_page;
        } else {
            unused_pages.insert(page_id);
        }
    }

    /**
     * @brief Read one page of data
     * @param page_id The page to be read
     * @param page_data Output buffer
     */
    void read_page(IdT page_id, char* page_data) {
        ++num_reads;
        peek_page(page_id, page_size, page_data);
    }

    /**
     * @brief Read only n bytes from page
     * @param page_id The page to be read
     * @param n Number of bytes, should be <= page_size
     * @param data Output buffer
     */
    void peek_page(IdT page_id, size_t n, char* data) {
        ++num_peeks;
        if (n > page_size) {
            return;
        }
        const auto offset = page_id * page_size;
        db_file.seekg(offset);
        db_file.read(data, n);
        if (db_file.fail()) {
            throw std::runtime_error("Bad read");
        }
    }

    void write_page(IdT page_id, const char* page_data) {
        ++num_writes;
        const auto offset = page_id * page_size;
        db_file.clear();
        db_file.seekp(offset);
        if (db_file.fail()) {
            throw std::runtime_error("Unable to seek");
        }
        db_file.write(page_data, page_size);
        db_file.flush();
        if (db_file.fail()) {
            throw std::runtime_error("Unable to seek");
        }
    }

    void reset_stats() {
        num_reads = num_peeks = num_writes = 0;
    }
};


#endif //DISKMANAGER_HPP
