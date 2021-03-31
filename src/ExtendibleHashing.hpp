#ifndef EXTENDIBLEHASHING_HPP
#define EXTENDIBLEHASHING_HPP

#include "common.h"
#include <bitset>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <functional>
#include "Bucket.hpp"
#include "HashingScheme.hpp"
#include "DiskManager.hpp"
#include <ranges>

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

    uint32_t get_sibling_idx(const uint32_t bucket_idx, const uint32_t local_depth) {
        // flip the 'local_depth' bit
        return bucket_idx ^ (1 << (local_depth - 1));
    }

    /**
     * Grow directory by doubling its size
     */
    void grow() {
        buckets.reserve(1 << ++global_depth);
        std::copy(buckets.begin(), buckets.end(), std::back_inserter(buckets));
    }

    /**
     * Shrink directory by halving its size
     * @return True if directory was shrunk
     */
    bool shrink() {
        if (!global_depth)
            return false;
        for (auto &bucket:buckets) {
            if (bucket->local_depth == global_depth)
                // can't shrink
                return false;
        }
        // simply truncate the second half of the vector
        buckets.resize(1 << --global_depth);
        return true;
    }

    /**
     * @brief Determine if the given bucket can be merged with some other bucket
     * For now this only happens if the bucket is fully empty.
     * But we can have other merging policies, for example, if the total size of bucket and its sibling is less than
     * the maximum, merge them.
     */
    bool can_combine(std::shared_ptr<Bucket<K, V>> bucket, uint32_t bucket_idx) {
        if (!bucket->is_empty())
            return false;
        const uint32_t sibling_idx = get_sibling_idx(bucket_idx, bucket->local_depth);
        auto sibling = buckets[sibling_idx];
        if (sibling->local_depth != bucket->local_depth) {
            // can't merge buckets with unequal depths
            return false;
        }
        return true;
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

    /**
     * @brief Remove the key-value entry
     * @return True if entry was found and removed
     */
    bool remove(const K &key) override {
        const uint32_t bucket_idx = get_bucket_idx(key);
        auto bucket = buckets[bucket_idx];
        if (!bucket->remove(key))
            // not found
            return false;

        while (global_depth > 0 && can_combine(bucket, bucket_idx)) {
            auto sibling = buckets[get_sibling_idx(bucket_idx, bucket->local_depth)];
            // move all remaining values from bucket into its sibling
            // TODO: The merging policy should tell us whether to move from bucket to sibling or vice versa
            // the merging policy should try to minimize these moves as much as possible
            for (auto &[key, value]:bucket->read_page()) {
                sibling->insert(key, value);
            }
            // replace all occurrences of bucket in the directory with its sibling, effectively deleting it
            for (auto &buck:buckets) {
                if (buck == bucket) {
                    buck = sibling;
                }
            }
            --sibling->local_depth;

            // try to halve the directory
            if (!shrink()) {
                // couldn't shrink
                break;
            }
            bucket = sibling;  // redo the process with the newly merged bucket
        }
        return true;
    }

    /**
     * @brief Generate the dot notation graph for the current structure
     * This can then be viewed using a GraphViz application.
     * @param out Output stream
     */
    void display(std::ostream &out) {
        out << "digraph G {\n"  // directed graph
            << "\trankdir=\"LR\";\n"  // ensure buckets are vertical
            << "\tnode [shape = record]\n";  // for array-like presentation of each bucket

        // generate the main directory
        out << "\tsubgraph directory {\n"
            << "\t\tarray [label=\"";
        const uint32_t max = (1 << global_depth) - 1;
        for (int i = 0; i <= max; ++i) {
            // "a0" is a label for the first element of the directory, used later to draw edges
            // `{0:0{1}b}` ensures that we print exactly `global_depth` number of bits, padded by 0s to the left
            fmt::print(out, "<a{0}> {0:0{1}b}{2}", i, global_depth, (i == max ? "" : " | "));
        }
        out << "\"];\n"
            << "\t}\n";

        // generate the buckets
        out << "\tsubgraph buckets {\n";
        std::unordered_set<std::shared_ptr<Bucket<K, V>>> done;
        for (auto &bucket:buckets) {
            if (done.contains(bucket))
                // generate each bucket exactly once
                continue;
            const std::unordered_map<K, V> items = bucket->read_page();
            fmt::print(out, "\t\tbucket{} [label=\"{}\", xlabel=\"{}\"];\n",
                       fmt::ptr(bucket),  // bucket ID, used later to draw edges
                       fmt::join(std::views::keys(items), "|"),  // bucket entries, only show keys
                       bucket->local_depth);
            done.insert(bucket);
        }
        out << "\t}\n";

        // generate edges from directory to buckets
        for (int i = 0; i < buckets.size(); ++i) {
            fmt::print(out, "\tarray:a{} -> bucket{};\n", i, fmt::ptr(buckets[i]));
        }
        out << "}\n";
    }
};

#endif //EXTENDIBLEHASHING_HPP
