#include <stdexcept>

#include "Board/Graph.h"
#include "Board/generated/Portless.h"
#include "Board/generated/Standard.h"
#include "Board/generated/Tall.h"
#include "Test/catch.h"

namespace k10engine {

namespace Board {

TEST_CASE("Graph initialization", "[Board][Board.Graph]")
{
    auto o = Orientation::Clock2Clock8;
    auto j = NodeType::Junction;

    SECTION("Trivial")
    {
        const auto g = Graph({}, {}, {});
        REQUIRE(g.size() == 0);
        REQUIRE(g.node(0) == nullptr);
    }

    SECTION("1 ocean, no edges")
    {
        const auto g = Graph({ NodeType::Ocean }, {}, {});
        auto n = g.node(0);
        REQUIRE(n != nullptr);
        REQUIRE(n->index() == 0);
        REQUIRE(n->type() == NodeType::Ocean);
        REQUIRE(!g.has_neighbor(n, Direction::Clock2));
        REQUIRE(!g.has_neighbor(n, Direction::Clock4));
        REQUIRE(!g.has_neighbor(n, Direction::Clock6));
        REQUIRE(!g.has_neighbor(n, Direction::Clock8));
        REQUIRE(!g.has_neighbor(n, Direction::Clock10));
        REQUIRE(!g.has_neighbor(n, Direction::Clock12));
        REQUIRE(g.neighbor(n, Direction::Clock2) == nullptr);
        REQUIRE(g.neighbor(n, Direction::Clock4) == nullptr);
        REQUIRE(g.neighbor(n, Direction::Clock6) == nullptr);
        REQUIRE(g.neighbor(n, Direction::Clock8) == nullptr);
        REQUIRE(g.neighbor(n, Direction::Clock10) == nullptr);
        REQUIRE(g.neighbor(n, Direction::Clock12) == nullptr);
    }

    SECTION("Edges without nodes should throw")
    {
        REQUIRE_THROWS_AS(Graph({}, { { 0, 1, Direction::Clock2 } }, {}), std::invalid_argument);
    }

    SECTION("Multiple edges in a Direction should throw")
    {
        REQUIRE_THROWS_AS(Graph({ NodeType::Ocean, NodeType::Junction, NodeType::Road },
                                { { 0, 1, Direction::Clock2 }, { 0, 2, Direction::Clock2 } },
                                {}),
                          std::invalid_argument);
    }

    SECTION("2 nodes, 1 edge")
    {
        const auto g =
            Graph({ NodeType::Ocean, NodeType::Junction }, { { 0, 1, Direction::Clock2 } }, {});
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
        REQUIRE_THROWS_AS(Graph({}, {}, { { 0, 1, o } }), std::invalid_argument);
    }

    SECTION("2 nodes, 1 port should throw")
    {
        REQUIRE_THROWS_AS(Graph({ j, j }, {}, { { 0, 1, o } }), std::invalid_argument);
    }

    SECTION("3 nodes, 1 port should throw if they're not 'connected'")
    {
        REQUIRE_THROWS_AS(Graph({ j, j, j }, {}, { { 0, 2, o } }), std::invalid_argument);
    }

    SECTION("3 nodes, 1 port")
    {
        auto g = Graph({ NodeType::Junction, NodeType::Road, NodeType::Junction },
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
        std::vector<NodeType> node_specs = {
            NodeType::Junction, NodeType::Road,         NodeType::Junction, NodeType::Hex,
            NodeType::Ocean,    NodeType::UnflippedHex, NodeType::Road,
        };
        auto g = Graph(node_specs,
                       { { 0, 1, Direction::Clock2 }, { 1, 2, Direction::Clock2 } },
                       { { 0, 2, Orientation::Clock2Clock8 } });
        for (int i = 3; i < 7; ++i) {
            REQUIRE_THROWS_AS(Graph(node_specs,
                                    { { 0, 1, Direction::Clock2 }, { 1, i, Direction::Clock2 } },
                                    { { 0, i, Orientation::Clock2Clock8 } }),
                              std::invalid_argument);
            REQUIRE_THROWS_AS(Graph(node_specs,
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
        auto g = get_standard_board();
        REQUIRE(g->size() == 163);
        // REQUIRE(b->ports().size() == 9);
        delete g;
    }

    SECTION("Portless")
    {
        auto g = get_portless_board();
        REQUIRE(g->size() == 163);
        // REQUIRE(b->ports().size() == 0);
        delete g;
    }

    SECTION("Tall")
    {
        auto g = get_tall_board();
        REQUIRE(g->size() == 295);
        // REQUIRE(b->ports().size() == 13);
        delete g;
    }
}

} // namespace Board

} // namespace k10engine
