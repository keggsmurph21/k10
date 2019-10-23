#pragma once

#include "Extras.h"
#include "HashTable.h"
#include "Optional.h"
#include "Vector.h"
#include <stdio.h>

namespace ADT {

template<typename K, typename V, typename KTraits = Traits<K>, typename VTraits = Traits<V>>
class HashMap {
private:
    struct Entry {
        K key;
        V value;
    };

    struct EntryTraits {
        static unsigned hash(const Entry& entry) { return KTraits::hash(entry.key); }
        static bool equals(const Entry& a, const Entry& b) { return KTraits::equals(a.key, b.key); }
        static void dump(const Entry& entry)
        {
            printf("key=");
            KTraits::dump(entry.key);
            printf(" value=");
            VTraits::dump(entry.value);
        }
    };

public:
    HashMap() {}

    bool is_empty() const { return m_table.is_empty(); }
    int size() const { return m_table.size(); }
    int capacity() const { return m_table.capacity(); }
    void clear() { m_table.clear(); }

    void set(const K& key, const V& value) { m_table.set({ key, value }); }
    void set(const K& key, V&& value) { m_table.set({ key, value }); }

    typedef HashTable<Entry, EntryTraits> HashTableType;
    typedef typename HashTableType::Iterator IteratorType;
    typedef typename HashTableType::ConstIterator ConstIteratorType;

    IteratorType begin() { return m_table.begin(); }
    IteratorType end() { return m_table.end(); }

    ConstIteratorType begin() const { return m_table.begin(); }
    ConstIteratorType end() const { return m_table.end(); }

    template<typename Finder>
    IteratorType find(unsigned hash, Finder finder)
    {
        return m_table.find(hash, finder);
    }

    template<typename Finder>
    ConstIteratorType find(unsigned hash, Finder finder) const
    {
        return m_table.find(hash, finder);
    }

    IteratorType find(const K& key)
    {
        return find(KTraits::hash(key),
                    [&](auto& entry) { return KTraits::equals(key, entry.key); });
    }

    ConstIteratorType find(const K& key) const
    {
        return find(KTraits::hash(key),
                    [&](auto& entry) { return KTraits::equals(key, entry.key); });
    }

    bool contains(const K& key) { return find(key) != end(); }

    void remove_one_randomly() { m_table.remove(m_table.begin()); }
    void remove(const K& key)
    {
        auto it = find(key);
        if (it != end())
            m_table.remove(it);
    }
    void remove(IteratorType it) { m_table.remove(it); }

    void ensure_capacity(int capacity) { m_table.ensure_capacity(capacity); }

    V& ensure(const K& key)
    {
        if (!contains(key))
            set(key, V());
        return find(key)->value;
    }

    Optional<typename Traits<V>::PeekType> get(const K& key) const
    {
        auto it = find(key);
        if (it == end())
            return {};
        return (*it).value;
    }

    Vector<K> keys() const
    {
        Vector<K> keys; // can i overload like this?
        keys.ensure_capacity(size());
        for (auto& it : *this)
            keys.unchecked_append(it.key);
        return keys;
    }

    Vector<V> values() const
    {
        Vector<V> values;
        values.ensure_capacity(size());
        for (auto& it : *this)
            values.unchecked_append(it.value);
        return values;
    }

    Vector<Entry> items() const
    {
        Vector<Entry> items;
        items.ensure_capacity(size());
        for (auto& it : *this)
            items.unchecked_append(it);
        return items;
    }

    void dump() { m_table.dump(); }

private:
    HashTableType m_table;
};

} // namespace ADT

using ADT::HashMap;
