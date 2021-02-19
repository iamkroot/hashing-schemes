#ifndef HASHINGSCHEME_HPP
#define HASHINGSCHEME_HPP

template<typename K, typename V>
class HashingScheme {
public:
    virtual bool insert(const K &key, const V &value) = 0;

    virtual bool get(const K &key, V* value) = 0;

    virtual bool remove(const K &key) = 0;
};


#endif //HASHINGSCHEME_HPP
