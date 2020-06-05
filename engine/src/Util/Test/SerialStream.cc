#include <cassert>

#include "Test/catch.h"
#include "Util/SerialStream.h"

TEST_CASE("Simple values", "[Util] [Util.SerialStream]")
{
    SECTION("char")
    {
        SerialStream ss;
        char expected{ 'x' };
        char actual;
        ss << expected;
        REQUIRE(ss.size() == sizeof(char));
        ss >> actual;
        REQUIRE(expected == actual);
    }
}

TEST_CASE("Primitive containers", "[Util] [Util.SerialStream]")
{
    SECTION("std::vector<char>")
    {
        SerialStream ss;
        std::vector<char> expected{ 'h', 'e', 'l', 'l', 'o' };
        std::vector<char> actual;
        ss << expected;
        ss >> actual;
        REQUIRE(expected == actual);
    }

    SECTION("std::unordered_map<char, size_t>")
    {
        SerialStream ss;
        std::unordered_map<char, int> expected{ { 'x', 0xdeadbeef }, { 'y', 0xfadeb00b } };
        std::unordered_map<char, int> actual;
        ss << expected;
        ss >> actual;
        REQUIRE(expected == actual);
    }
}

class Point {
    SERIALIZABLE(Point)
public:
    Point(int x, int y)
        : m_x(x)
        , m_y(y)
    {
    }
    bool operator==(const Point& o) const { return m_x == o.m_x && m_y == o.m_y; }

private:
    int m_x;
    int m_y;
};

SerialStream& operator<<(SerialStream& ss, const Point& p)
{
    ss << p.m_x;
    ss << p.m_y;
    return ss;
}

Point Point::deserialize(SerialStream& ss)
{
    int x;
    int y;
    ss >> x;
    ss >> y;
    return { x, y };
}

TEST_CASE("Simple class", "[Util] [Util.SerialStream]")
{
    SECTION("Point{ int, int }")
    {
        SerialStream ss;
        Point p{ 6, 9 };
        REQUIRE(p == Point::deserialize(ss << p));
    }
}

enum class Color {
    Red,
    Blue,
    Green,
    Yellow,
};

struct SingleColor {
    Color color;

    INLINE_SERIALIZABLE(SingleColor)
};

SingleColor::operator u64() const
{
    return static_cast<u64>(color);
}

SingleColor SingleColor::operator=(u64 bytes)
{
    color = static_cast<Color>(bytes);
    return *this;
}

struct DoubleColor {
    Color c0;
    Color c1;

    bool operator==(const DoubleColor& other) const { return c0 == other.c0 && c1 == other.c1; }

    INLINE_SERIALIZABLE(DoubleColor)
};

DoubleColor::operator u64() const
{
    return static_cast<u64>(c0) | (static_cast<u64>(c1) << 2);
}

DoubleColor DoubleColor::operator=(u64 bytes)
{
    c0 = static_cast<Color>(bytes & 0b11);
    c1 = static_cast<Color>((bytes & (0b11 << 2)) >> 2);
    return *this;
}

struct SexdecupleColor {
    Color c0;
    Color c1;
    Color c2;
    Color c3;
    Color c4;
    Color c5;
    Color c6;
    Color c7;
    Color c8;
    Color c9;
    Color c10;
    Color c11;
    Color c12;
    Color c13;
    Color c14;
    Color c15;

    INLINE_SERIALIZABLE(SexdecupleColor)
};

SexdecupleColor::operator u64() const
{
    return static_cast<u8>(c0) | (static_cast<u8>(c1) << 2) | (static_cast<u8>(c2) << 4) | (static_cast<u8>(c3) << 6)
           | (static_cast<u8>(c4) << 8) | (static_cast<u8>(c5) << 10) | (static_cast<u8>(c6) << 12)
           | (static_cast<u8>(c7) << 14) | (static_cast<u8>(c8) << 16) | (static_cast<u8>(c9) << 18)
           | (static_cast<u8>(c10) << 20) | (static_cast<u8>(c11) << 22) | (static_cast<u8>(c12) << 24)
           | (static_cast<u8>(c13) << 26) | (static_cast<u8>(c14) << 28) | (static_cast<u8>(c15) << 30);
}

SexdecupleColor SexdecupleColor::operator=(u64 bytes)
{
    c0 = static_cast<Color>(bytes & 0b11);
    c1 = static_cast<Color>((bytes & (0b11 << (2))) >> 2);
    c2 = static_cast<Color>((bytes & (0b11 << (4))) >> 4);
    c3 = static_cast<Color>((bytes & (0b11 << (6))) >> 6);
    c4 = static_cast<Color>((bytes & (0b11 << (8))) >> 8);
    c5 = static_cast<Color>((bytes & (0b11 << (10))) >> 10);
    c6 = static_cast<Color>((bytes & (0b11 << (12))) >> 12);
    c7 = static_cast<Color>((bytes & (0b11 << (14))) >> 14);
    c8 = static_cast<Color>((bytes & (0b11 << (16))) >> 16);
    c9 = static_cast<Color>((bytes & (0b11 << (18))) >> 18);
    c10 = static_cast<Color>((bytes & (0b11 << (20))) >> 20);
    c11 = static_cast<Color>((bytes & (0b11 << (22))) >> 22);
    c12 = static_cast<Color>((bytes & (0b11 << (24))) >> 24);
    c13 = static_cast<Color>((bytes & (0b11 << (26))) >> 26);
    c14 = static_cast<Color>((bytes & (0b11 << (28))) >> 28);
    c15 = static_cast<Color>((bytes & (0b11 << (30))) >> 30);
    return *this;
}

TEST_CASE("Bitpacked structs", "[Util] [Util.SerialStream]")
{
    SECTION("SingleColor")
    {
        SerialStream ss;
        SingleColor expected{ Color::Blue };
        SingleColor actual;
        ss << expected;
        ss >> actual;
        REQUIRE(expected.color == actual.color);
    }

    SECTION("DoubleColor")
    {
        SerialStream ss;
        DoubleColor expected{ Color::Blue, Color::Yellow };
        DoubleColor actual;
        ss << expected;
        ss >> actual;
        REQUIRE(expected.c0 == actual.c0);
        REQUIRE(expected.c1 == actual.c1);
    }

    SECTION("SexdecupleColor")
    {
        SerialStream ss;
        SexdecupleColor expected{ Color::Blue,  Color::Red,    Color::Blue,   Color::Blue,   Color::Blue, Color::Blue,
                                  Color::Red,   Color::Yellow, Color::Yellow, Color::Yellow, Color::Blue, Color::Red,
                                  Color::Green, Color::Green,  Color::Green,  Color::Blue };
        SexdecupleColor actual;
        ss << expected;
        ss >> actual;
        REQUIRE(expected.c0 == actual.c0);
        REQUIRE(expected.c1 == actual.c1);
        REQUIRE(expected.c2 == actual.c2);
        REQUIRE(expected.c3 == actual.c3);
        REQUIRE(expected.c4 == actual.c4);
        REQUIRE(expected.c5 == actual.c5);
        REQUIRE(expected.c6 == actual.c6);
        REQUIRE(expected.c7 == actual.c7);
        REQUIRE(expected.c8 == actual.c8);
        REQUIRE(expected.c9 == actual.c9);
        REQUIRE(expected.c10 == actual.c10);
        REQUIRE(expected.c11 == actual.c11);
        REQUIRE(expected.c12 == actual.c12);
        REQUIRE(expected.c13 == actual.c13);
        REQUIRE(expected.c14 == actual.c14);
        REQUIRE(expected.c15 == actual.c15);
    }
}

TEST_CASE("Bitpacked struct containers", "[Util] [Util.Serializable]")
{
    SECTION("std::vector<SingleColor>")
    {
        SerialStream ss;
        std::vector<SingleColor> expected{ { Color::Blue }, { Color::Yellow }, { Color::Red } };
        std::vector<SingleColor> actual;
        ss << expected;
        ss >> actual;
        REQUIRE(expected.size() == actual.size());
        for (size_t i = 0; i < expected.size(); ++i) {
            REQUIRE(expected[i].color == actual[i].color);
        }
    }

    SECTION("std::vector<SexdecupleColor>")
    {
        SerialStream ss;
        std::vector<SexdecupleColor> expected{ { Color::Blue,
                                                 Color::Red,
                                                 Color::Blue,
                                                 Color::Blue,
                                                 Color::Blue,
                                                 Color::Blue,
                                                 Color::Red,
                                                 Color::Yellow,
                                                 Color::Yellow,
                                                 Color::Yellow,
                                                 Color::Blue,
                                                 Color::Red,
                                                 Color::Green,
                                                 Color::Green,
                                                 Color::Green,
                                                 Color::Blue },
                                               { Color::Red,
                                                 Color::Blue,
                                                 Color::Blue,
                                                 Color::Blue,
                                                 Color::Blue,
                                                 Color::Yellow,
                                                 Color::Red,
                                                 Color::Yellow,
                                                 Color::Yellow,
                                                 Color::Red,
                                                 Color::Blue,
                                                 Color::Red,
                                                 Color::Green,
                                                 Color::Green,
                                                 Color::Yellow,
                                                 Color::Blue } };
        std::vector<SexdecupleColor> actual;
        ss << expected;
        ss >> actual;
        REQUIRE(expected.size() == actual.size());
        for (size_t i = 0; i < expected.size(); ++i) {
            REQUIRE(expected[i].c0 == actual[i].c0);
            REQUIRE(expected[i].c1 == actual[i].c1);
            REQUIRE(expected[i].c2 == actual[i].c2);
            REQUIRE(expected[i].c3 == actual[i].c3);
            REQUIRE(expected[i].c4 == actual[i].c4);
            REQUIRE(expected[i].c5 == actual[i].c5);
            REQUIRE(expected[i].c6 == actual[i].c6);
            REQUIRE(expected[i].c7 == actual[i].c7);
            REQUIRE(expected[i].c8 == actual[i].c8);
            REQUIRE(expected[i].c9 == actual[i].c9);
            REQUIRE(expected[i].c10 == actual[i].c10);
            REQUIRE(expected[i].c11 == actual[i].c11);
            REQUIRE(expected[i].c12 == actual[i].c12);
            REQUIRE(expected[i].c13 == actual[i].c13);
            REQUIRE(expected[i].c14 == actual[i].c14);
            REQUIRE(expected[i].c15 == actual[i].c15);
        }
    }
}

class Container {
    SERIALIZABLE(Container)
public:
    Container(Point point, std::unordered_map<char, DoubleColor> colors_map)
        : m_point(point)
        , m_colors_map(std::move(colors_map))
    {
    }
    bool operator==(const Container& c) const { return m_point == c.m_point && m_colors_map == c.m_colors_map; }

private:
    Point m_point;
    std::unordered_map<char, DoubleColor> m_colors_map;
};

SerialStream& operator<<(SerialStream& ss, const Container& c)
{
    ss << c.m_point;
    ss << c.m_colors_map;
    return ss;
}

Container Container::deserialize(SerialStream& ss)
{
    Point point = Point::deserialize(ss);
    std::unordered_map<char, DoubleColor> colors_map;
    ss >> colors_map;
    return { point, colors_map };
}

TEST_CASE("Complex bitpacked class", "[Util] [Util.Serializable]")
{
    SECTION("Single Container")
    {
        SerialStream ss;
        Container c{ { 6, 9 }, { { 'x', { Color::Yellow, Color::Yellow } }, { 'z', { Color::Green, Color::Red } } } };
        REQUIRE(c == Container::deserialize(ss << c));
    }
}
/*

class Container {
    SERIALIZABLE(Container)

public:
    Container(u8 char_,
              u16 short_,
              u32 int_,
              size_t size_t_,
              std::vector<u8> char_vec,
              std::vector<size_t> size_t_vec,
              Color color,
              std::unordered_map<u8, u32> char_to_int)
        : m_char(char_)
        , m_short(short_)
        , m_int(int_)
        , m_size_t(size_t_)
        , m_char_vec(std::move(char_vec))
        , m_size_t_vec(std::move(size_t_vec))
        , m_color(color)
        , m_char_to_int(char_to_int)
    {
    }

    void dump() const
    {
        printf(
            "m_char = %#02x, m_short = %#04x, m_int = %#08x, m_size_t = %#016lx, m_char_vec = {
", m_char, m_short, m_int, m_size_t); for (const auto& ele : m_char_vec) printf("%#02x, ", ele);
        printf("}, m_size_t_vec = { ");
        for (const auto& ele : m_size_t_vec)
            printf("%#016lx, ", ele);
        printf("}, m_color = %d, m_char_to_int = { ", m_color);
        for (const auto& it : m_char_to_int)
            printf("%#02x => %#08x, ", it.first, it.second);
        printf("}\n");
    }

private:
    u8 m_char;
    u16 m_short;
    u32 m_int;
    size_t m_size_t;
    std::vector<u8> m_char_vec;
    std::vector<size_t> m_size_t_vec;
    Color m_color;
    std::unordered_map<u8, u32> m_char_to_int;
};

ColorStream& operator<<(ColorStream& ss, const Container& c)
{
    ss << c.m_char;
    ss << c.m_short;
    ss << c.m_int;
    ss << c.m_size_t;
    ss << c.m_char_vec;
    ss << c.m_size_t_vec;
    ss << c.m_color;
    ss << c.m_char_to_int;
    return ss;
}

Container Container::deserialize(ColorStream& ss)
{
    u8 char_;
    u16 short_;
    u32 int_;
    size_t size_t_;
    std::vector<u8> char_vec;
    std::vector<size_t> size_t_vec;
    Color color;
    std::unordered_map<u8, u32> char_to_int;
    ss >> char_;
    ss >> short_;
    ss >> int_;
    ss >> size_t_;
    ss >> char_vec;
    ss >> size_t_vec;
    ss >> color;
    ss >> char_to_int;
    return { char_, short_, int_, size_t_, char_vec, size_t_vec, color, char_to_int };
}

int main()
{
    ColorStream ss;
    Container c{ 69,
                 420,
                 100000,
                 0xbaa1024000400505,
                 { 0xde, 0xad, 0xbe, 0xef },
                 { 0xbababacc533b, 0xfaff0daff0 },
                 Color::Yellow,
                 { { 0xff, 0xfffffff }, { 0xab, 0xcdef0123 }, { 0x00, 0x31415962 }

                 } };

    c.dump();
    Container::deserialize(ss << c).dump();
}
*/
