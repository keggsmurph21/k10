#include <cassert>
#include <limits>

#include "Util/Encoder.h"

Encoder& Encoder::operator<<(bool value)
{
    *this << static_cast<u8>(value);
    return *this;
}

Encoder& Encoder::operator<<(char value)
{
    return *this << static_cast<u8>(value);
}

Encoder& Encoder::operator<<(i8 value)
{
    return *this << static_cast<u8>(value);
}

Encoder& Encoder::operator<<(i16 value)
{
    return *this << static_cast<u16>(value);
}

Encoder& Encoder::operator<<(i32 value)
{
    return *this << static_cast<u32>(value);
}

Encoder& Encoder::operator<<(i64 value)
{
    return *this << static_cast<u64>(value);
}

Encoder& Encoder::operator<<(u8 value)
{
    m_buffer.append(value);
    return *this;
}

Encoder& Encoder::operator<<(u16 value)
{
    m_buffer.grow_by(sizeof(value));
    m_buffer.append(static_cast<u8>(value >> 8));
    m_buffer.append(static_cast<u8>(value >> 0));
    return *this;
}

Encoder& Encoder::operator<<(u32 value)
{
    m_buffer.grow_by(sizeof(value));
    m_buffer.append(static_cast<u8>(value >> 24));
    m_buffer.append(static_cast<u8>(value >> 16));
    m_buffer.append(static_cast<u8>(value >> 8));
    m_buffer.append(static_cast<u8>(value >> 0));
    return *this;
}

Encoder& Encoder::operator<<(u64 value)
{
    m_buffer.grow_by(sizeof(value));
    m_buffer.append(static_cast<u8>(value >> 56));
    m_buffer.append(static_cast<u8>(value >> 48));
    m_buffer.append(static_cast<u8>(value >> 40));
    m_buffer.append(static_cast<u8>(value >> 32));
    m_buffer.append(static_cast<u8>(value >> 24));
    m_buffer.append(static_cast<u8>(value >> 16));
    m_buffer.append(static_cast<u8>(value >> 8));
    m_buffer.append(static_cast<u8>(value >> 0));
    return *this;
}

Encoder& Encoder::operator<<(size_t value)
{
    return *this << static_cast<u64>(value);
}

Encoder& Encoder::operator<<(const std::string& value)
{
    assert(value.size() <= std::numeric_limits<u8>::max());
    *this << static_cast<u8>(value.size());
    for (auto ch : value)
        *this << ch;
    return *this;
}
