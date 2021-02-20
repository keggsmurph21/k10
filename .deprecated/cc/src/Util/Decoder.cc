#include <vector>

#include "Util/Decoder.h"

bool Decoder::decode(bool& value)
{
    u8 byte;
    if (!decode(byte))
        return false;
    if (byte > 0x01)
        return false;
    value = (byte == 0x01);
    return true;
}

bool Decoder::decode(char& value)
{
    (void)value;
    assert(false);
}

bool Decoder::decode(i8& value)
{
    (void)value;
    assert(false);
}

bool Decoder::decode(i16& value)
{
    (void)value;
    assert(false);
}

bool Decoder::decode(i32& value)
{
    (void)value;
    assert(false);
}

bool Decoder::decode(i64& value)
{
    (void)value;
    assert(false);
}

bool Decoder::decode(u8& value)
{
    if (m_buffer.unread_size() < sizeof(value))
        return false;
    value = m_buffer.read();
    return true;
}

bool Decoder::decode(u16& value)
{
    if (m_buffer.unread_size() < sizeof(value))
        return false;
    value = 0;
    value |= static_cast<u16>(m_buffer.read()) << 8;
    value |= static_cast<u16>(m_buffer.read()) << 0;
    return true;
}

bool Decoder::decode(u32& value)
{
    if (m_buffer.unread_size() < sizeof(value))
        return false;
    value = 0;
    value |= static_cast<u32>(m_buffer.read()) << 24;
    value |= static_cast<u32>(m_buffer.read()) << 16;
    value |= static_cast<u32>(m_buffer.read()) << 8;
    value |= static_cast<u32>(m_buffer.read()) << 0;
    return true;
}

bool Decoder::decode(u64& value)
{
    if (m_buffer.unread_size() < sizeof(value))
        return false;
    value = 0;
    value |= static_cast<u64>(m_buffer.read()) << 56;
    value |= static_cast<u64>(m_buffer.read()) << 48;
    value |= static_cast<u64>(m_buffer.read()) << 40;
    value |= static_cast<u64>(m_buffer.read()) << 32;
    value |= static_cast<u64>(m_buffer.read()) << 24;
    value |= static_cast<u64>(m_buffer.read()) << 16;
    value |= static_cast<u64>(m_buffer.read()) << 8;
    value |= static_cast<u64>(m_buffer.read()) << 0;
    return true;
}

bool Decoder::decode(size_t& value)
{
    u64 bytes;
    if (!decode(bytes))
        return false;
    value = static_cast<size_t>(bytes);
    return true;
}

bool Decoder::decode(std::string& value)
{
    u8 len; // NB: This prevents having strings >255 chars long!
    if (!decode(len))
        return false;
    if (m_buffer.unread_size() < len)
        return false;

    std::vector<char> chars;
    chars.reserve(len);
    for (auto i = 0; i < len; ++i)
        chars.push_back(static_cast<char>(m_buffer.read()));
    value = { chars.begin(), chars.end() };
    return true;
}
