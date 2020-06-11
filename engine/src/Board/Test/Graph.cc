#include <stdexcept>

#include "Board/Graph.h"
#include "Board/Parser.h"
#include "Test/catch.h"

namespace k10engine::Board {

TEST_CASE("Graph initialization", "[Board][Board.Graph]")
{
    auto o = Orientation::Clock2Clock8;
    auto j = NodeType::Junction;

    SECTION("Trivial")
    {
        const auto g = Graph({ 0, 0 }, {}, {}, {});
        REQUIRE(g.size() == 0);
        REQUIRE(g.node(0) == nullptr);
    }

    SECTION("1 ocean, no edges")
    {
        const auto g = Graph({ 1, 1 }, { { 0, 0, NodeType::Ocean } }, {}, {});
        auto n = g.node(0);
        REQUIRE(n != nullptr);
        REQUIRE(n->index() == 0);
        REQUIRE(n->x() == 0);
        REQUIRE(n->y() == 0);
        REQUIRE(n->type() == NodeType::Ocean);
        REQUIRE(!g.has_neighbor(*n, Direction::Clock2));
        REQUIRE(!g.has_neighbor(*n, Direction::Clock4));
        REQUIRE(!g.has_neighbor(*n, Direction::Clock6));
        REQUIRE(!g.has_neighbor(*n, Direction::Clock8));
        REQUIRE(!g.has_neighbor(*n, Direction::Clock10));
        REQUIRE(!g.has_neighbor(*n, Direction::Clock12));
        REQUIRE(g.neighbor(*n, Direction::Clock2) == nullptr);
        REQUIRE(g.neighbor(*n, Direction::Clock4) == nullptr);
        REQUIRE(g.neighbor(*n, Direction::Clock6) == nullptr);
        REQUIRE(g.neighbor(*n, Direction::Clock8) == nullptr);
        REQUIRE(g.neighbor(*n, Direction::Clock10) == nullptr);
        REQUIRE(g.neighbor(*n, Direction::Clock12) == nullptr);
    }

    SECTION("Edges without nodes should throw")
    {
        REQUIRE_THROWS_AS(Graph({ 0, 0 }, {}, { { 0, 1, Direction::Clock2 } }, {}), std::invalid_argument);
    }

    SECTION("Multiple edges in a Direction should throw")
    {
        REQUIRE_THROWS_AS(Graph({ 3, 3 },
                                { { 0, 0, NodeType::Ocean }, { 1, 1, NodeType::Junction }, { 2, 2, NodeType::Road } },
                                { { 0, 1, Direction::Clock2 }, { 0, 2, Direction::Clock2 } },
                                {}),
                          std::invalid_argument);
    }

    SECTION("2 nodes, 1 edge")
    {
        const auto g = Graph(
            { 2, 2 }, { { 0, 0, NodeType::Ocean }, { 1, 1, NodeType::Junction } }, { { 0, 1, Direction::Clock2 } }, {});
        auto n0 = g.node(0);
        REQUIRE(n0 != nullptr);
        REQUIRE(n0->index() == 0);
        REQUIRE(n0->type() == NodeType::Ocean);
        auto n1 = g.node(1);
        REQUIRE(n1 != nullptr);
        REQUIRE(n1->index() == 1);
        REQUIRE(n1->type() == NodeType::Junction);
    }

    SECTION("0 nodes, 1 port should throw")
    {
        REQUIRE_THROWS_AS(Graph({ 0, 0 }, {}, {}, { { 0, 1, o } }), std::invalid_argument);
    }

    SECTION("2 nodes, 1 port should throw")
    {
        REQUIRE_THROWS_AS(Graph({ 2, 2 }, { { 0, 0, j }, { 1, 1, j } }, {}, { { 0, 1, o } }), std::invalid_argument);
    }

    SECTION("3 nodes, 1 port should throw if they're not 'connected'")
    {
        REQUIRE_THROWS_AS(Graph({ 3, 3 }, { { 0, 0, j }, { 1, 1, j }, { 2, 2, j } }, {}, { { 0, 2, o } }),
                          std::invalid_argument);
    }

    SECTION("3 nodes, 1 port")
    {
        auto g = Graph({ 3, 3 },
                       { { 0, 0, NodeType::Junction }, { 1, 1, NodeType::Road }, { 2, 2, NodeType::Junction } },
                       { { 0, 1, Direction::Clock2 },
                         { 1, 0, Direction::Clock8 },
                         { 1, 2, Direction::Clock2 },
                         { 2, 1, Direction::Clock8 } },
                       { { 0, 2, Orientation::Clock2Clock8 } });
        REQUIRE(g.size() == 3);
        // REQUIRE(g.size() == 1);
    }

    SECTION("Constructing Port with invalid NodeTypes should throw")
    {
        NodeSpecs node_specs = {
            { 0, 0, NodeType::Junction }, { 1, 1, NodeType::Road },  { 2, 2, NodeType::Junction },
            { 3, 3, NodeType::Hex },      { 4, 4, NodeType::Ocean }, { 5, 5, NodeType::UnflippedHex },
            { 6, 6, NodeType::Road },
        };
        auto g = Graph({ 7, 7 },
                       node_specs,
                       { { 0, 1, Direction::Clock2 }, { 1, 2, Direction::Clock2 } },
                       { { 0, 2, Orientation::Clock2Clock8 } });
        for (size_t i = 3; i < 7; ++i) {
            REQUIRE_THROWS_AS(Graph({ 7, 7 },
                                    node_specs,
                                    { { 0, 1, Direction::Clock2 }, { 1, i, Direction::Clock2 } },
                                    { { 0, i, Orientation::Clock2Clock8 } }),
                              std::invalid_argument);
            REQUIRE_THROWS_AS(Graph({ 7, 7 },
                                    node_specs,
                                    { { 0, 1, Direction::Clock8 }, { 1, i, Direction::Clock8 } },
                                    { { 0, i, Orientation::Clock2Clock8 } }),
                              std::invalid_argument);
        }
    }
}

TEST_CASE("Loading generated boards", "[Board][Board.Graph]")
{
    SECTION("Standard")
    {
        auto g = from_file("static/Standard.board");
        REQUIRE(g.size() == 163);
        // REQUIRE(b->ports().size() == 9);
    }

    SECTION("Portless")
    {
        auto g = from_file("static/Portless.board");
        REQUIRE(g.size() == 163);
        // REQUIRE(b->ports().size() == 0);
    }

    SECTION("Tall")
    {
        auto g = from_file("static/Tall.board");
        REQUIRE(g.size() == 295);
        // REQUIRE(b->ports().size() == 13);
    }
}

} // namespace k10engine::Board
