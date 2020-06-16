#pragma once

#include <cassert>
#include <iostream>

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

    ElementType& operator*() const { return m_node->m_value; }
    ElementType* operator->() const { return &m_node->m_value; }

    bool is_end() const { return *this == end(); }
    static DoublyLinkedListIterator end() { return DoublyLinkedListIterator(nullptr); }

private:
    friend ListType;
    explicit DoublyLinkedListIterator(typename ListType::Node* node)
        : m_node(node)
    {
    }
    typename ListType::Node* m_node;
};

template<typename T>
class DoublyLinkedList {
private:
    struct Node {
        explicit Node(const T& value)
            : m_value(value)
        {
        }
        explicit Node(T&& value)
            : m_value(std::move(value))
        {
        }
        Node* prev{ nullptr };
        Node* next{ nullptr };
        T m_value;
    };

public:
    DoublyLinkedList() {}
    ~DoublyLinkedList() { clear(); }

    T& head()
    {
        assert(!is_empty());
        return m_head->m_value;
    }
    T& tail()
    {
        assert(!is_empty());
        return m_tail->m_value;
    }

    bool is_empty() const { return m_head == nullptr; }

    using Iterator = DoublyLinkedListIterator<DoublyLinkedList, T>;
    friend Iterator;

    Iterator find(const T& value)
    {
        for (auto it = begin(); it != end(); ++it) {
            if (*it == value) {
                return it;
            }
        }
        return end();
    }
    Iterator begin() { return Iterator(m_head); }
    Iterator end() { return Iterator::end(); }

    using ConstIterator = DoublyLinkedListIterator<const DoublyLinkedList, const T>;
    friend ConstIterator;

    ConstIterator find(const T& value) const
    {
        for (auto it = begin(); it != end(); ++it) {
            if (*it == value) {
                return it;
            }
        }
        return end();
    }
    ConstIterator begin() const { return ConstIterator(m_head); }
    ConstIterator end() const { return ConstIterator::end(); }

    bool contains(const T& value) const { return !find(value).is_end(); }

    void insert_after(Iterator, const T&);
    void insert_after(Iterator, T&&);
    void prepend(const T& value)
    {
        auto node = new Node(value);
        node->next = m_head;
        if (m_head != nullptr) {
            m_head->prev = node;
        } else {
            m_tail = node;
        }
        m_head = node;
    }
    void prepend(T&& value)
    {
        auto node = new Node(std::move(value));
        node->next = m_head;
        if (m_head != nullptr) {
            m_head->prev = node;
        } else {
            m_tail = node;
        }
        m_head = node;
    }
    void append(const T& value)
    {
        auto node = new Node(value);
        node->prev = m_tail;
        if (m_tail != nullptr) {
            m_tail->next = node;
        } else {
            m_head = node;
        }
        m_tail = node;
    }
    void append(T&& value)
    {
        auto node = new Node(std::move(value));
        node->prev = m_tail;
        if (m_tail != nullptr) {
            m_tail->next = node;
        } else {
            m_head = node;
        }
        m_tail = node;
    }

    void clear()
    {
        if (is_empty()) {
            return;
        }
        auto* node = m_head;
        while (node != nullptr) {
            auto* next = node->next;
            delete node;
            node = next;
        }
        m_head = nullptr;
        m_tail = nullptr;
    }
    void remove(Iterator it)
    {
        assert(!it.is_end());
        auto* node = it.m_node;
        if (node->prev == nullptr) {
            m_head = node->next;
        } else {
            node->prev->next = node->next;
        }
        if (node->next == nullptr) {
            m_tail = node->prev;
        } else {
            node->next->prev = node->prev;
        }
        delete node;
    }
    T pop()
    {
        assert(!is_empty());
        T value = tail();
        Iterator it = begin();
        while (it.m_node->next != nullptr) {
            ++it;
        }
        remove(it);
        return value;
    }

    template<typename U>
    friend std::ostream& operator<<(std::ostream&, const DoublyLinkedList<U>&);

private:
    Node* m_head{ nullptr };
    Node* m_tail{ nullptr };
};

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const DoublyLinkedList<T>& list)
{
    if (list.is_empty()) {
        os << "List{}";
        return os;
    }
    bool is_first = true;
    os << "List{ ";
    for (auto& node : list) {
        if (!is_first) {
            os << ", ";
        }
        is_first = false;
        os << node;
    }
    os << " }";
    return os;
}
