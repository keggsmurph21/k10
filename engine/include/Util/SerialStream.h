#pragma once

#include <cassert>
#include <queue>
#include <stdio.h>
#include <unordered_map>
#include <vector>

#include "Util/Types.h"

#define CHECK_CONTAINER_SIZE(container) assert((container).size() < (1 << (8 * sizeof(SizeType))));

#define SERIALIZABLE(T)                                       \
public:                                                       \
    friend SerialStream& operator<<(SerialStream&, const T&); \
    static T deserialize(SerialStream&);

#define INLINE_SERIALIZABLE(T) \
private:                       \
    friend class SerialStream; \
    operator u64() const;      \
    T operator=(u64 bytes);

class SerialStream {
public:
    using SizeType = u16;

    template<typename T>
    SerialStream& operator<<(T);
    template<typename T>
    SerialStream& operator>>(T&);

    template<typename T>
    SerialStream& operator<<(const std::vector<T>&);
    template<typename T>
    SerialStream& operator>>(std::vector<T>&);

    template<typename K, typename V>
    SerialStream& operator<<(const std::unordered_map<K, V>&);
    template<typename K, typename V>
    SerialStream& operator>>(std::unordered_map<K, V>&);

    size_t size() const { return m_bytes.size(); }
    void dump()
    {
        u8 byte;
        for (size_t i = 0; i < size(); ++i) {
            byte = m_bytes.front();
            printf("%#02x ", byte);
            m_bytes.push(byte);
            m_bytes.pop();
        }
        puts("\n");
    }

protected:
    void push_bytes(u64 value, u8 width)
    {
        for (u8 shift = 0; shift < width; ++shift) {
            m_bytes.push(value >> (shift * 8));
        }
    }

    u64 pop_bytes(u8 width)
    {
        assert(size() >= width);
        u64 value = 0;
        for (u8 shift = 0; shift < width; ++shift) {
            u64 byte = m_bytes.front();
            m_bytes.pop();
            value |= byte << (shift * 8);
        }
        return value;
    }

    std::queue<u8> m_bytes;
};

template<typename T>
SerialStream& SerialStream::operator<<(T value)
{
    push_bytes(value, sizeof(T));
    return *this;
}

template<typename T>
SerialStream& SerialStream::operator>>(T& value)
{
    value = pop_bytes(sizeof(T));
    return *this;
}

template<typename T>
SerialStream& SerialStream::operator<<(const std::vector<T>& vector)
{
    CHECK_CONTAINER_SIZE(vector);
    *this << static_cast<SizeType>(vector.size());
    for (const auto& element : vector)
        *this << element;
    return *this;
}

template<typename T>
SerialStream& SerialStream::operator>>(std::vector<T>& vector)
{
    SizeType length;
    *this >> length;
    vector.clear();
    vector.resize(length);
    for (SizeType index = 0; index < length; ++index)
        *this >> vector[index];
    return *this;
}

template<typename K, typename V>
SerialStream& SerialStream::operator<<(const std::unordered_map<K, V>& map)
{
    CHECK_CONTAINER_SIZE(map);
    *this << static_cast<SizeType>(map.size());
    for (const auto& it : map) {
        *this << it.first;
        *this << it.second;
    }
    return *this;
}

template<typename K, typename V>
SerialStream& SerialStream::operator>>(std::unordered_map<K, V>& map)
{
    SizeType length;
    *this >> length;
    map.clear();
    map.reserve(length);
    for (SizeType index = 0; index < length; ++index) {
        K key;
        *this >> key;
        *this >> map[key];
    }
    return *this;
}
