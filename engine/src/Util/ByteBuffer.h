#pragma once

#include <iomanip>
#include <iostream>
#include <vector>

#include "Util/Types.h"

class ByteBuffer {
    MAKE_NONCOPYABLE(ByteBuffer)
public:
    ByteBuffer() {}
    ByteBuffer(const char* bytes, size_t size)
    {
        grow_by(size);
        for (size_t i = 0; i < size; ++i)
            append(*bytes++);
    }
    ByteBuffer(ByteBuffer&& moved_buf)
        : m_bytes(std::move(moved_buf.m_bytes))
        , m_cursor(moved_buf.m_cursor)
    {
    }
    ByteBuffer operator=(ByteBuffer&& moved_buf) { return std::move(moved_buf); }

    size_t unread_size() { return m_bytes.size() - m_cursor; }
    u8 read() { return m_bytes.at(m_cursor++); }
    void append(u8 byte) { m_bytes.push_back(byte); }
    void grow_by(size_t size) { m_bytes.reserve(m_bytes.size() + size); }

    void clear()
    {
        m_bytes.clear();
        m_cursor = 0;
    }

    u8* data() { return m_bytes.data(); }

    void concat(const char* bytes, size_t size)
    {
        grow_by(size);
        for (size_t i = 0; i < size; ++i)
            append(*bytes++);
    }

    friend std::ostream& operator<<(std::ostream& os, const ByteBuffer& buf)
    {
        if (buf.m_bytes.empty()) {
            os << "{}";
            return os;
        }
        os << "{ ";
        for (const auto& byte : buf.m_bytes)
            os << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(byte) << " ";
        os << "}";
        return os;
    }

private:
    std::vector<u8> m_bytes;
    size_t m_cursor{ 0 };
};
