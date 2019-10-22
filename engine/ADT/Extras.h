#pragma once

#include "HashFunctions.h"
#include <stdio.h>

namespace ADT {

template<typename T>
T&& move(T& arg)
{
    return static_cast<T&&>(arg);
}

template<typename T>
class TemporaryChange {
public:
    TemporaryChange(T& variable, T value)
        : m_variable(variable)
        , m_old_value(variable)
    {
        m_variable = value;
    }
    ~TemporaryChange() { m_variable = m_old_value; }

private:
    T& m_variable;
    T m_old_value;
};

template<typename T>
struct GenericTraits {
    using PeekType = T;
    static constexpr bool is_trivial() { return false; }
    static bool equals(const T& a, const T& b) { return a == b; }
    static void dump(T value) { printf("%p", (void*)&value); }
};

template<typename T>
struct Traits : public GenericTraits<T> {
};

template<>
struct Traits<bool> : public GenericTraits<bool> {
    static constexpr bool is_trivial() { return true; }
    static unsigned hash(bool b) { return int_hash(b); }
    static void dump(bool b) { printf(b ? "true" : "false"); }
};

template<>
struct Traits<int> : public GenericTraits<int> {
    static constexpr bool is_trivial() { return true; }
    static unsigned hash(int i) { return int_hash(i); }
    static void dump(int i) { printf("%d", i); }
};

template<>
struct Traits<unsigned> : public GenericTraits<unsigned> {
    static constexpr bool is_trivial() { return true; }
    static unsigned hash(unsigned u) { return int_hash(u); }
    static void dump(unsigned u) { printf("%u", u); }
};

template<>
struct Traits<char> : public GenericTraits<char> {
    static constexpr bool is_trivial() { return true; }
    static unsigned hash(char c) { return int_hash(c); }
    static void dump(char c) { printf("%c", c); }
};

template<typename T>
struct Traits<T*> : public GenericTraits<T*> {
    static constexpr bool is_trivial() { return true; }
    static unsigned hash(const T* p) { return int_hash((unsigned)p); }
    static void dump(T* p) { printf("%p", p); }
    static bool equals(const T* a, const T* b) { return a == b; }
};

} // namespace ADT
