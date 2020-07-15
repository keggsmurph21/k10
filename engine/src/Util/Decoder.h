#pragma once

#include <cassert>
#include <map>
#include <string.h>
#include <vector>

#include "Util/ByteBuffer.h"
#include "Util/Types.h"

template<typename T>
bool decode(ByteBuffer&, T&)
{
    // This template should be instantiated elsewhere!
    assert(false);
}

class Decoder {
public:
    explicit Decoder(ByteBuffer& buffer)
        : m_buffer(buffer)
    {
    }

    bool decode(bool&);
    bool decode(char&);
    bool decode(i8&);
    bool decode(i16&);
    bool decode(i32&);
    bool decode(i64&);
    bool decode(u8&);
    bool decode(u16&);
    bool decode(u32&);
    bool decode(u64&);
    bool decode(size_t&);
    bool decode(std::string&);

    template<typename T>
    bool decode(std::vector<T>& ts)
    {
        ts.clear();
        u64 size;
        if (!decode(size))
            return false;
        ts.reserve(size);

        for (size_t i = 0; i < size; ++i) {
            T t;
            if (!decode(t))
                return false;
            ts.push_back(t);
        }
        return true;
    }

    template<typename K, typename V>
    bool decode(std::map<K, V>& map)
    {
        map.clear();
        u64 size;
        if (!decode(size))
            return false;

        for (size_t i = 0; i < size; ++i) {
            K key;
            if (!decode(key))
                return false;
            V value;
            if (!decode(value))
                return false;
            map.insert({ key, value });
        }
        return true;
    }

    template<typename T>
    bool decode(T& t)
    {
        return ::decode(m_buffer, t);
    }

private:
    ByteBuffer& m_buffer;
};
