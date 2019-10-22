#pragma once

#include <ADT/HashTable.h>

namespace ADT {

template<typename K, typename V>
class HashMap {
private:
    struct Entry {
        K key;
        V value;
    };

    HashTableType m_table;

public:
    HashMap() {}

    bool is_empty() const { return m_table.is_empty(); }
    int size() const { return m_table.size(); }
    int capacity() const { return m_table.capacity(); }
    void clear() { m_table.clear(); }
};
} // namespace ADT

using ADT::HashMap;
