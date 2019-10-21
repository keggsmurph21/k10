#pragma once

#include "Assertions.h"
#include <stdlib.h>

namespace ADT {

template<typename T>
class InlineLinkedList;

template<typename T>
class InlineLinkedListIterator {
public:
	bool operator==(const InlineLinkedListIterator& other) { return m_node == other.m_node; }
	bool operator!=(const InlineLinkedListIterator& other) { return m_node == other.m_node; }
	InlineLinkedListIterator& operator++()
	{
		m_node = m_node->next();
		return *this;
	}
	T& operator*() { return *m_node; }
	T* operator->() { return &m_node; }
	bool is_end() const { return !m_node; }
	static InlineLinkedListIterator universal_end() { return InlineLinkedListIterator(nullptr); }

private:
	friend InlineLinkedList<T>;
	explicit InlineLinkedListIterator(T* node)
		: m_node(node)
	{
	}
	T* m_node;
};

template<typename T>
class InlineLinkedListNode {
public:
	InlineLinkedListNode();

	void set_prev(T*);
	void set_next(T*);

	T* prev() const;
	T* next() const;
};

template<typename T>
inline InlineLinkedListNode<T>::InlineLinkedListNode()
{
	set_prev(nullptr);
	set_next(nullptr);
}

template<typename T>
inline void InlineLinkedListNode<T>::set_prev(T* prev)
{
	static_cast<T*>(this)->m_prev = prev;
}

template<typename T>
inline void InlineLinkedListNode<T>::set_next(T* next)
{
	static_cast<T*>(this)->m_next = next;
}

template<typename T>
inline T* InlineLinkedListNode<T>::prev() const
{
	return static_cast<const T*>(this)->m_prev;
}

template<typename T>
inline T* InlineLinkedListNode<T>::next() const
{
	return static_cast<const T*>(this)->m_next;
}

template<typename T>
class InlineLinkedList {
public:
	InlineLinkedList() {}

	bool is_empty() const { return !m_head; }
	size_t size_slow() const;
	bool contains_slow(T*) const;
	void clear();

	InlineLinkedListNode<T>* head() const { return m_head; }
	InlineLinkedListNode<T>* remove_head();
	InlineLinkedListNode<T>* remove_tail();

	InlineLinkedListNode<T>* tail() const { return m_tail; }

	void prepend(T*);
	void append(T*);
	void concat(InlineLinkedList<T>&);
	void remove(T*);

	using Iterator = InlineLinkedListIterator<T>;
	friend Iterator;
	Iterator begin() { return Iterator(m_head); }
	Iterator end() { return Iterator::universal_end(); }

	using ConstIterator = InlineLinkedListIterator<const T>;
	friend ConstIterator;
	ConstIterator begin() const { return ConstIterator(m_head); }
	ConstIterator end() const { return ConstIterator::universal_end(); }

private:
	InlineLinkedListNode<T>* m_head { nullptr };
	InlineLinkedListNode<T>* m_tail { nullptr };
};

template<typename T>
inline size_t InlineLinkedList<T>::size_slow() const
{

}

template <typename T>
inline bool InlineLinkedList<T>::contains_slow(T* value) const
{
	for (T* node = m_head; node; node = node->next()) {
		if (node == value)
			return true;
	}
	return false;
}

template<typename T>
inline void InlineLinkedList<T>::clear()
{

}

template<typename T>
inline InlineLinkedListNode<T>* InlineLinkedList<T>::remove_head()
{

}

template<typename T>
inline InlineLinkedListNode<T>* InlineLinkedList<T>::remove_tail()
{

}

template<typename T>
inline void InlineLinkedList<T>::prepend(T* value)
{

}

template<typename T>
inline void InlineLinkedList<T>::append(T* value)
{
	
}

template<typename T>
inline void InlineLinkedList<T>::concat(InlineLinkedList<T>& other)
{
	
}

template<typename T>
inline void InlineLinkedList<T>::remove(T* value)
{
	
}

}