#include "Test/catch.h"
#include "Util/Decoder.h"
#include "Util/Encoder.h"

struct Point {
    u8 x;
    u8 y;
};

template<>
void encode(ByteBuffer& buf, const Point& p)
{
    Encoder encoder(buf);
    encoder << p.x;
    encoder << p.y;
}

template<>
bool decode(ByteBuffer& buf, Point& p)
{
    Decoder decoder(buf);
    u8 x;
    if (!decoder.decode(x))
        return false;
    p.x = x;

    u8 y;
    if (!decoder.decode(y))
        return false;
    p.y = y;

    return true;
}

TEST_CASE("Decoder", "[Util][Util.Decoder]")
{
    SECTION("vector<u8>")
    {
        SECTION("Empty")
        {
            ByteBuffer buf("\0\0\0\0\0\0\0\0", 8);
            Decoder decoder(buf);
            std::vector<u8> u8s;
            REQUIRE(decoder.decode(u8s));
            REQUIRE(u8s.size() == 0);
        }
        SECTION("3 elements")
        {
            ByteBuffer buf("\0\0\0\0\0\0\0\3\4\2\0", 11);
            Decoder decoder(buf);
            std::vector<u8> u8s;
            REQUIRE(decoder.decode(u8s));
            REQUIRE(u8s.size() == 3);
            REQUIRE(u8s.at(0) == 4);
            REQUIRE(u8s.at(1) == 2);
            REQUIRE(u8s.at(2) == 0);
        }
    }
    SECTION("vector<Point>")
    {
        SECTION("2 elements")
        {
            ByteBuffer buf("\0\0\0\0\0\0\0\2\4\2\0\7", 12);
            Decoder decoder(buf);
            std::vector<Point> points;
            REQUIRE(decoder.decode(points));
            REQUIRE(points.size() == 2);
            REQUIRE(points.at(0).x == 4);
            REQUIRE(points.at(0).y == 2);
            REQUIRE(points.at(1).x == 0);
            REQUIRE(points.at(1).y == 7);
        }
    }
    SECTION("map<std::string, Point>")
    {
        ByteBuffer buf;
        Encoder encoder(buf);
        std::map<std::string, Point> expected{ { "a", { 1, 2 } }, { "b", { 3, 4 } } };
        encoder << expected;

        Decoder decoder(buf);
        std::map<std::string, Point> actual;
        REQUIRE(decoder.decode(actual));
        REQUIRE(actual.size() == expected.size());
        REQUIRE(actual.at("a").x == expected.at("a").x);
        REQUIRE(actual.at("a").y == expected.at("a").y);
        REQUIRE(actual.at("b").x == expected.at("b").x);
        REQUIRE(actual.at("b").y == expected.at("b").y);
    }
}
