#include <stdexcept>

#include "Board/Base.h"
#include "Board/generated/Portless.h"
#include "Board/generated/Standard.h"
#include "Board/generated/Tall.h"
#include "Test/catch.h"

using namespace k10engine::Board;

TEST_CASE("Board initialization", "[Board][Board.Board]")
{
    auto o = Orientation::Clock2Clock8;
    auto j = NodeType::Junction;

    SECTION("Trivial")
    {
        auto b = Base("test", new Graph({}, {}), {});
        REQUIRE(b.name() == "test");
        REQUIRE(const_cast<Graph*>(b.graph())->nodes().size() == 0);
        REQUIRE(b.ports().size() == 0);
    }

    SECTION("0 nodes, 1 port should throw")
    {
        REQUIRE_THROWS_AS(Base("test", new Graph({}, {}), { { 0, 1, o } }), std::invalid_argument);
    }

    SECTION("2 nodes, 1 port should throw")
    {
        REQUIRE_THROWS_AS(Base("test", new Graph({ { 0, j }, { 1, j } }, {}), { { 0, 1, o } }),
                          std::invalid_argument);
    }

    SECTION("3 nodes, 1 port should throw if they're not 'connected'")
    {
        REQUIRE_THROWS_AS(
            Base("test", new Graph({ { 0, j }, { 1, j }, { 2, j } }, {}), { { 0, 2, o } }),
            std::invalid_argument);
    }

    SECTION("3 nodes, 1 port")
    {
        auto b = Base(
            "test",
            new Graph(
                { { 1, NodeType::Junction }, { 2, NodeType::Road }, { 3, NodeType::Junction } },
                { { 1, 2, Direction::Clock2 },
                  { 2, 1, Direction::Clock8 },
                  { 2, 3, Direction::Clock2 },
                  { 3, 2, Direction::Clock8 } }),
            { { 1, 3, Orientation::Clock2Clock8 } });
        REQUIRE(b.name() == "test");
        REQUIRE(const_cast<Graph*>(b.graph())->nodes().size() == 3);
        REQUIRE(b.ports().size() == 1);
    }
}

TEST_CASE("Loading generated boards", "[Board][Board.Board]")
{
    SECTION("Standard")
    {
        auto b = get_standard_board();
        REQUIRE(b->name() == "Standard");
        REQUIRE(const_cast<Graph*>(b->graph())->nodes().size() == 163);
        REQUIRE(b->ports().size() == 9);
        delete b;
    }

    SECTION("Portless")
    {
        auto b = get_portless_board();
        REQUIRE(b->name() == "Portless");
        REQUIRE(const_cast<Graph*>(b->graph())->nodes().size() == 163);
        REQUIRE(b->ports().size() == 0);
        delete b;
    }

    SECTION("Tall")
    {
        auto b = get_tall_board();
        REQUIRE(b->name() == "Tall");
        REQUIRE(const_cast<Graph*>(b->graph())->nodes().size() == 295);
        REQUIRE(b->ports().size() == 13);
        delete b;
    }
}
