#pragma once

#include <cassert>
#include <optional>
#include <string.h>

#include "Util/ByteBuffer.h"
#include "Util/Types.h"

template<typename T>
void encode(ByteBuffer&, T&)
{
    // This method should be overridden elsewhere!
    assert(false);
}

class Encoder {
public:
    explicit Encoder(ByteBuffer& buffer)
        : m_buffer(buffer)
    {
    }

    Encoder& operator<<(bool);
    Encoder& operator<<(char);
    Encoder& operator<<(i8);
    Encoder& operator<<(i16);
    Encoder& operator<<(i32);
    Encoder& operator<<(i64);
    Encoder& operator<<(u8);
    Encoder& operator<<(u16);
    Encoder& operator<<(u32);
    Encoder& operator<<(u64);
    Encoder& operator<<(size_t);
    Encoder& operator<<(const std::string&);

    template<typename T>
    Encoder& operator<<(const std::vector<T>& ts)
    {
        *this << static_cast<u64>(ts.size());
        for (auto& t : ts)
            *this << t;
        return *this;
    }

    template<typename T>
    Encoder& operator<<(const std::optional<T>& optional)
    {
        *this << optional.has_value();
        if (optional.has_value())
            *this << optional.value();
        return *this;
    }

    template<typename T>
    Encoder& operator<<(const T& t)
    {
        ::encode(m_buffer, t);
        return *this;
    }

private:
    ByteBuffer& m_buffer;
};
