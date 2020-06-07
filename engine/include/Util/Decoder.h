#pragma once

#include <cassert>
#include <string.h>

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
    bool decode(T& t)
    {
        return ::decode(m_buffer, t);
    }

private:
    ByteBuffer& m_buffer;
};
