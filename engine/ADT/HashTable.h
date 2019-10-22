#pragma once

#include "Extras.h"
#include "HashFunctions.h"
#include "SinglyLinkedList.h"
#include <stdio.h>

namespace ADT {

template<typename T, typename = Traits<T>>
class HashTable;

template<typename HashTableType, typename ElementType, typename BucketIteratorType>
class HashTableIterator {
public:
    bool operator!=(const HashTableIterator& other) const
    {
        if (m_is_end && other.m_is_end)
            return false;
        return &m_table != &other.m_table || m_is_end != other.m_is_end
               || m_bucket_index != other.m_bucket_index
               || m_bucket_iterator != other.m_bucket_iterator;
    }
    bool operator==(const HashTableIterator& other) const { return !(*this != other); }
    ElementType& operator*() { return *m_bucket_iterator; }
    ElementType* operator->() { return m_bucket_iterator.operator->(); }
    HashTableIterator& operator++()
    {
        skip_to_next();
        return *this;
    }

    void skip_to_next()
    {
        while (!m_is_end) {
            if (m_bucket_iterator.is_end()) {
                ++m_bucket_index;
                if (m_bucket_index >= m_table.capacity()) {
                    m_is_end = true;
                    return;
                }
                m_bucket_iterator = m_table.bucket(m_bucket_index).begin();
            } else {
                ++m_bucket_iterator;
            }
            if (!m_bucket_iterator.is_end())
                return;
        }
    }

private:
    friend HashTableType;

    explicit HashTableIterator(
        HashTableType& table,
        bool is_end,
        BucketIteratorType bucket_iterator = BucketIteratorType::universal_end(),
        int bucket_index = 0)
        : m_table(table)
        , m_bucket_index(bucket_index)
        , m_is_end(is_end)
        , m_bucket_iterator(bucket_iterator)
    {
        ASSERT(!table.m_clearing);
        ASSERT(!table.m_rehashing);
        if (!is_end & !m_table.is_empty()
            && !(m_bucket_iterator != BucketIteratorType::universal_end())) {
            m_bucket_iterator = m_table.bucket(0).begin();
            if (m_bucket_iterator.is_end())
                skip_to_next();
        }
    }

    HashTableType& m_table;
    int m_bucket_index{ 0 };
    bool m_is_end{ false };
    BucketIteratorType m_bucket_iterator;
};

template<typename T, typename TTraits>
class HashTable {
private:
    using Bucket = SinglyLinkedList<T>;

public:
    HashTable() {}
    HashTable(const HashTable& other)
    {
        ensure_capacity(other.size());
        for (auto& it : other)
            set(it);
    }
    HashTable(HashTable&& other)
        : m_buckets(other.m_buckets)
        , m_size(other.m_size)
        , m_capacity(other.m_capacity)
    {
        other.m_size = 0;
        other.m_capacity = 0;
        other.m_buckets = nullptr;
    }
    ~HashTable() { clear(); }

    HashTable& operator=(const HashTable& other)
    {
        if (this != &other) {
            clear();
            ensure_capacity(other.size());
            for (auto& it : other)
                set(it);
        }
        return *this;
    }
    HashTable& operator=(HashTable&& other)
    {
        if (this != &other) {
            clear();
            m_buckets = other.m_buckets;
            m_size = other.m_size;
            m_capacity = other.m_capacity;
            other.m_size = 0;
            other.m_capacity = 0;
            other.m_buckets = nullptr;
        }
        return *this;
    }

    bool is_empty() const { return !m_size; }
    int size() const { return m_size; }
    int capacity() const { return m_capacity; }

    void ensure_capacity(int capacity)
    {
        ASSERT(capacity >= size()); // could this just be an early return?
        rehash(capacity);
    }

    void set(T&& value)
    {
        if (!m_capacity)
            rehash(1);
        auto& bucket = lookup(value);
        for (auto& e : bucket) {
            if (TTraits::equals(e, value)) {
                e = move(value);
                return;
            }
        }
        if (size() >= capacity()) {
            rehash(size() + 1);
            insert(move(value));
        } else {
            bucket.append(move(value));
        }
        m_size++;
    }
    void set(const T& value)
    {
        if (!m_capacity)
            rehash(1);
        auto& bucket = lookup(value);
        for (auto& e : bucket) {
            if (TTraits::equals(e, value)) {
                e = move(value);
                return;
            }
        }
        if (size() >= capacity()) {
            rehash(size() + 1);
            insert(move(value));
        } else {
            bucket.append(move(value));
        }
        m_size++;
    }
    bool contains(const T&) const;
    void clear()
    {
        TemporaryChange<bool> change(m_clearing, true);
        if (m_buckets) {
            delete[] m_buckets;
            m_buckets = nullptr;
        }
        m_capacity = 0;
        m_size = 0;
    }

    void dump() const
    {
        printf("HashTable{%p} m_size=%u, m_capacity=%u, m_buckets=%p\n",
               (void*)this,
               m_size,
               m_capacity,
               (void*)m_buckets);
        for (int i = 0; i < m_capacity; ++i) {
            auto& bucket = m_buckets[i];
            printf("Bucket%u\n", i);
            for (auto& e : bucket) {
                printf("  > ");
                TTraits::dump(e);
                printf("\n");
            }
        }
    }

    using Iterator = HashTableIterator<HashTable, T, typename Bucket::Iterator>;
    friend Iterator;
    Iterator begin() { return Iterator(*this, is_empty()); }
    Iterator end() { return Iterator(*this, true); }

    using ConstIterator =
        HashTableIterator<const HashTable, const T, typename Bucket::ConstIterator>;
    friend ConstIterator;
    ConstIterator begin() const { return ConstIterator(*this, is_empty()); }
    ConstIterator end() const { return ConstIterator(*this, true); }

    template<typename Finder>
    Iterator find(unsigned hash, Finder finder)
    {
        if (is_empty())
            return end();
        int bucket_index;
        auto& bucket = lookup_with_hash(hash, &bucket_index);
        auto bucket_iterator = bucket.find(finder);
        if (bucket_iterator != bucket.end())
            return Iterator(*this, false, bucket_iterator, bucket_index);
        return end();
    }

    template<typename Finder>
    ConstIterator find(unsigned hash, Finder finder) const
    {
        if (is_empty())
            return end();
        int bucket_index;
        auto& bucket = lookup_with_hash(hash, &bucket_index);
        auto bucket_iterator = bucket.find(finder);
        if (bucket_iterator != bucket.end())
            return ConstIterator(*this, false, bucket_iterator, bucket_index);
        return end();
    }

    Iterator find(const T& value)
    {
        return find(TTraits::hash(value),
                    [&](auto& other) { return TTraits::equals(value, other); });
    }

    ConstIterator find(const T& value) const
    {
        return find(TTraits::hash(value),
                    [&](auto& other) { return TTraits::equals(value, other); });
    }

    void remove(const T& value)
    {
        auto it = find(value);
        if (it != end())
            remove(it);
    }
    void remove(Iterator it)
    {
        ASSERT(!is_empty());
        m_buckets[it.m_bucket_index].remove(it.m_bucket_iterator);
        --m_size;
    }

private:
    Bucket& lookup(const T&, int* bucket_index = nullptr);
    const Bucket& lookup(const T&, int* bucket_index = nullptr) const;

    Bucket& lookup_with_hash(unsigned hash, int* bucket_index)
    {
        if (bucket_index)
            *bucket_index = hash % m_capacity;
        return m_buckets[hash % m_capacity];
    }
    const Bucket& lookup_with_hash(unsigned hash, int* bucket_index) const
    {
        if (bucket_index)
            *bucket_index = hash % m_capacity;
        return m_buckets[hash % m_capacity];
    }

    void rehash(int new_capacity)
    {
        TemporaryChange<bool> change(m_rehashing, true);
        new_capacity *= 2;
        auto* new_buckets = new Bucket[new_capacity];
        auto* old_buckets = m_buckets;
        int old_capacity = m_capacity;
        m_buckets = new_buckets;
        m_capacity = new_capacity;

        for (int i = 0; i < old_capacity; ++i) {
            for (auto& value : old_buckets[i]) {
                insert(move(value));
            }
        }

        delete[] old_buckets;
    }

    void insert(const T& value)
    {
        auto& bucket = lookup(value);
        bucket.append(move(value));
    }
    void insert(T&& value)
    {
        auto& bucket = lookup(value);
        bucket.append(move(value));
    }

    Bucket& bucket(int index) { return m_buckets[index]; }
    const Bucket& bucket(int index) const { return m_buckets[index]; }

    Bucket* m_buckets{ nullptr };

    int m_size{ 0 };
    int m_capacity{ 0 };
    bool m_clearing{ false };
    bool m_rehashing{ false };
};

template<typename T, typename TraitsForT>
auto HashTable<T, TraitsForT>::lookup(const T& value, int* bucket_index) -> Bucket&
{
    unsigned hash = TraitsForT::hash(value);
    return lookup_with_hash(hash, bucket_index);
}

template<typename T, typename TraitsForT>
auto HashTable<T, TraitsForT>::lookup(const T& value, int* bucket_index) const -> const Bucket&
{
    unsigned hash = TraitsForT::hash(value);
    return lookup_with_hash(hash, bucket_index);
}

template<typename T, typename TTraits>
bool HashTable<T, TTraits>::contains(const T& value) const
{
    if (is_empty())
        return false;
    auto& bucket = lookup(value);
    for (auto& e : bucket) {
        if (TTraits::equals(e, value))
            return true;
    }
    return false;
}

} // namespace ADT

using ADT::HashTable;
