#pragma once

#define MAKE_NONCOPYABLE(Clazz)   \
private:                          \
    Clazz(const Clazz&) = delete; \
    Clazz operator=(const Clazz&) = delete;

using i8 = signed char;
using u8 = unsigned char;
using i16 = signed short;
using u16 = unsigned short;
using i32 = signed int;
using u32 = unsigned int;
using i64 = signed long long;
using u64 = unsigned long long;
