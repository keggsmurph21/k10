#pragma once

#include "SinglyLinkedList.h"

namespace ADT {

template<typename T>
class HashTable;

template<typename HashTableType, typename ElementType, typename BucketIteratorType>
class HashTableIterator;

template<typename T>
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
	HashTable(HashTable&& other)
		: m_buckets(other.m_buckets)
		, m_size(other.m_size)
		, m_capacity(other.m_capacity)
	{
		other.m_size = 0;
		other.m_capacity = 0;
		other.m_buckets = nullptr;
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
	~HashTable() { clear(); }

	bool is_empty() const { return !m_size; }
	int size() const { return m_size; }
	int capacity() const { return m_capacity; }

	void ensure_capacity(int capacity);
	void set(const T&);
	void set(T&&);
	bool contains(const T&) const;
	void clear();

	void rehash(int capacity);

private:
	Bucket* m_buckets { nullptr };

	int m_size { 0 };
	int m_capacity { 0 };
};


template <typename T>
inline void HashTable<T>::ensure_capacity(int capacity)
{
	ASSERT(capacity >= size());
	rehash(capacity);
}

template <typename T>
inline void HashTable<T>::rehash(int new_capacity)
{
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

}

using ADT::HashTable;