#pragma once

#include "Extras.h"
#include <new>

namespace ADT {

template<typename T>
class alignas(T) Optional {
public:
    Optional() {}
    Optional(const T& value)
        : m_has_value(true)
    {
        new (&m_storage) T(value);
    }
    template<typename U>
    Optional(const U& value)
        : m_has_value(true)
    {
        new (&m_storage) T(value);
    }
    Optional(T&& value)
        : m_has_value(true)
    {
        new (&m_storage) T(move(value));
    }

    ~Optional() { clear(); }

    void clear()
    {
        if (m_has_value) {
            value().~T();
            m_has_value = false;
        }
    }

    T& value()
    {
        ASSERT(m_has_value);
        return *reinterpret_cast<T*>(&m_storage);
    }

    bool has_value() { return m_has_value; }

private:
    // call this when we don't want to alter consume state
    const T& raw_value() const
    {
        ASSERT(m_has_value);
        return *reinterpret_cast<const T*>(&m_storage);
    }
    char m_storage[sizeof(T)];
    bool m_has_value{ false };
};

} // namespace ADT
