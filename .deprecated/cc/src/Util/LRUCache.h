#pragma once

#include <functional>

#include "Util/DoublyLinkedList.h"

template<typename Key, typename Value>
class LRUCache {
private:
    struct Node {
        Node(const Key& k, Value* v)
            : key(k)
            , value(v)
        {
        }
        ~Node() { delete value; }
        Key key;
        Value* value;
    };

    static void noop(Key&, Value*) {}

    using Storage = DoublyLinkedList<Node*>;

public:
    LRUCache(std::function<Value*(const Key&)> on_miss, size_t capacity)
        : m_on_miss(on_miss)
        , m_on_eviction(noop)
        , m_capacity(capacity)
        , m_size(0)
    {
        assert(capacity > 0);
    }
    LRUCache(std::function<Value*(const Key&)> on_miss, std::function<void(Key&, Value*)> on_eviction, size_t capacity)
        : m_on_miss(on_miss)
        , m_on_eviction(on_eviction)
        , m_capacity(capacity)
        , m_size(0)
    {
        assert(capacity > 0);
    }

    ~LRUCache() { clear(); }

    Value* get(const Key& key)
    {
        auto it = find(key);
        if (!it.is_end()) {
            auto* value = (*it)->value;
            touch(it);
            return value;
        }
        auto* value = m_on_miss(key);
        if (value == nullptr)
            return nullptr;
        prepend(key, value);
        return value;
    }

    void set(const Key& key, Value* value)
    {
        assert(value != nullptr);
        auto it = find(key);
        if (!it.is_end()) {
            touch(it);
            (*it)->value = value;
            return;
        }
        prepend(key, value);
    }

    size_t size() const { return m_size; }
    size_t capacity() const { return m_capacity; }

    void clear()
    {
        for (auto* node : m_storage) {
            delete node;
        }
    }

private:
    std::function<Value*(const Key&)> m_on_miss;
    std::function<void(Key&, Value*)> m_on_eviction;
    size_t m_capacity;

    Storage m_storage;
    size_t m_size;

    void touch(typename Storage::Iterator it)
    {
        if (it == m_storage.begin())
            return;
        auto* node = *it;
        m_storage.remove(it);
        m_storage.prepend(node);
    }

    void prepend(const Key& key, Value* value)
    {
        if (m_size == m_capacity) {
            auto* evicted_node = m_storage.pop();
            m_on_eviction(evicted_node->key, evicted_node->value);
            delete evicted_node;
            --m_size;
        }
        auto* node = new Node(key, value);
        m_storage.prepend(node);
        ++m_size;
    }

    typename Storage::Iterator find(const Key& key)
    {
        for (auto it = m_storage.begin(); !it.is_end(); ++it) {
            if ((*it)->key == key)
                return it;
        }
        return m_storage.end();
    }
};
