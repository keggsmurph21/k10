#pragma once

#include "Extras.h"

namespace ADT {

template<typename T>
class Point {
public:
    Point() {}
    Point(T x, T y)
        : m_x(x)
        , m_y(y)
    {
    }
    bool operator==(Point&& other) { return x() == other.x() && y() == other.y(); }
    bool operator==(const Point& other) const { return x() == other.x() && y() == other.y(); }

    T x() const { return m_x; }
    T y() const { return m_y; }

private:
    T m_x{ 0 };
    T m_y{ 0 };
};

template<>
struct Traits<Point<double>> : public GenericTraits<Point<double>> {
    static constexpr bool is_trivial() { return false; }
    static unsigned hash(Point<double> p)
    {
        // Cantor's enumeration
        // https://en.wikipedia.org/wiki/Pairing_function#Cantor_pairing_function
        return (unsigned)((p.x() + p.y()) * (p.x() + p.y() + 1) / 2 + p.y());
    }
    static void dump(Point<double> p) { printf("Point(%f,%f)", p.x(), p.y()); }
};

template<>
struct Traits<Point<int>> : public GenericTraits<Point<int>> {
    static constexpr bool is_trivial() { return false; }
    static unsigned hash(Point<int> p)
    {
        // Cantor's enumeration
        // https://en.wikipedia.org/wiki/Pairing_function#Cantor_pairing_function
        return (unsigned)((p.x() + p.y()) * (p.x() + p.y() + 1) / 2 + p.y());
    }
    static void dump(Point<int> p) { printf("Point(%d,%d)", p.x(), p.y()); }
};

} // namespace ADT
