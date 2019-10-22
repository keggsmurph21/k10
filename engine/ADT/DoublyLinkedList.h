#pragma once

#include "Assertions.h"
#include "Extras.h"

namespace ADT {

template<typename ListType, typename ElementType>
class DoublyLinkedListIterator {
public:
    bool operator==(const DoublyLinkedListIterator& other) const { return m_node == other.m_node; }
    bool operator!=(const DoublyLinkedListIterator& other) const { return m_node != other.m_node; }
    DoublyLinkedListIterator& operator++()
    {
        m_node = m_node->next;
        return *this;
    }
    ElementType& operator*() { return m_node->value; }
    ElementType* operator->() { return &m_node->value; }
    bool is_end() const { return !m_node; }
    static DoublyLinkedListIterator universal_end() { return DoublyLinkedListIterator(nullptr); }

private:
    friend ListType;
    explicit DoublyLinkedListIterator(typename ListType::Node* node)
        : m_node(node)
    {
    }
    typename ListType::Node* m_node{ nullptr };
};

template<typename T>
class DoublyLinkedList {
private:
    struct Node {
        explicit Node(T&& v)
            : value(move(v))
        {
        }
        explicit Node(const T& v)
            : value(v)
        {
        }
        T value;
        Node* next{ nullptr };
        Node* prev{ nullptr };
    };

public:
    DoublyLinkedList() {}
    ~DoublyLinkedList() { clear(); }

    bool is_empty() const { return !head(); }

    inline int size_slow() const
    {
        int size = 0;
        for (auto* node = m_head; node; node = node->next)
            ++size;
        return size;
    }

    void clear()
    {
        for (auto* node = m_head; node;) {
            auto* next = node->next;
            delete node;
            node = next;
        }
        m_head = nullptr;
        m_tail = nullptr;
    }

    T& first()
    {
        ASSERT(head());
        return head()->value;
    }
    const T& first() const
    {
        ASSERT(head());
        return head()->value;
    }
    T& last()
    {
        ASSERT(tail());
        return tail()->value;
    }
    const T& last() const
    {
        ASSERT(tail());
        return tail()->value;
    }

    T take_first()
    {
        ASSERT(m_head);
        auto* prev_head = m_head;
        T value = move(first());
        if (m_tail == m_head)
            m_tail = nullptr;
        m_head = m_head->next;
        m_head->prev = nullptr;
        delete prev_head;
        return value;
    }

    T take_last()
    {
        ASSERT(m_tail);
        auto* prev_tail = m_tail;
        T value = move(last());
        if (m_head == m_tail)
            m_head = nullptr;
        m_tail = m_tail->prev;
        m_tail->next = nullptr;
        delete prev_tail;
        return value;
    }

    void append(T&& value)
    {
        auto* node = new Node(move(value));
        if (!m_head) {
            m_head = node;
            m_tail = node;
            return;
        }
        node->prev = m_tail;
        m_tail->next = node;
        m_tail = node;
    }

    void append(const T& value)
    {
        auto* node = new Node(value);
        if (!m_head) {
            m_head = node;
            m_tail = node;
            return;
        }
        node->prev = m_tail;
        m_tail->next = node;
        m_tail = node;
    }

    void prepend(T&& value)
    {
        auto* node = new Node(move(value));
        if (!m_head) {
            m_head = node;
            m_tail = node;
            return;
        }
        node->next = m_head;
        m_head->prev = node;
        m_head = node;
    }

    void prepend(const T& value)
    {
        auto* node = new Node(value);
        if (!m_head) {
            m_head = node;
            m_tail = node;
            return;
        }
        node->next = m_head;
        m_head->prev = node;
        m_head = node;
    }

    bool contains_slow(const T& value) const
    {
        for (auto* node = m_head; node; node = node->next) {
            if (node->value == value)
                return true;
        }
        return false;
    }

    using Iterator = DoublyLinkedListIterator<DoublyLinkedList, T>;
    friend Iterator;
    Iterator begin() { return Iterator(m_head); }
    Iterator end() { return Iterator::universal_end(); }

    using ConstIterator = DoublyLinkedListIterator<const DoublyLinkedList, const T>;
    friend ConstIterator;
    ConstIterator begin() const { return ConstIterator(m_head); }
    ConstIterator end() const { return ConstIterator::universal_end(); }

    Iterator find(const T& value)
    {
        for (auto* node = m_head; node; node = node->next) {
            if (node->value == value)
                return Iterator(node);
        }
        return end();
    }

    ConstIterator find(const T& value) const
    {
        for (auto* node = m_head; node; node = node->next) {
            if (node->value == value)
                return ConstIterator(node);
        }
        return end();
    }

    void remove(const Iterator& it)
    {
        ASSERT(!it.is_end());
        auto* node = it.m_node;
        if (node->prev) {
            ASSERT(node != m_head);
            node->prev->next = node->next;
        } else {
            ASSERT(node == m_head);
            m_head = node->next;
        }
        if (node->next) {
            ASSERT(node != m_tail);
            node->next->prev = node->prev;
        } else {
            ASSERT(node == m_tail);
            m_tail = node->prev;
        }
        delete node;
    }

private:
    Node* head() { return m_head; }
    const Node* head() const { return m_head; }

    Node* tail() { return m_tail; }
    const Node* tail() const { return m_tail; }

    Node* m_head{ nullptr };
    Node* m_tail{ nullptr };
};

} // namespace ADT

using ADT::DoublyLinkedList;
