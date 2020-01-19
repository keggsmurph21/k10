#include <stdexcept>

#include "Board/Graph.h"
#include "Test/catch.h"

using namespace k10engine::Board;

TEST_CASE("Graph initialization", "[Board][Board.Graph]")
{
    SECTION("Trivial")
    {
        auto g = Graph({}, {});
        REQUIRE(g.nodes().size() == 0);
        REQUIRE(g.num_hexes() == 0);
        REQUIRE(g.num_junctions() == 0);
        REQUIRE(g.num_oceans() == 0);
        REQUIRE(g.num_roads() == 0);
        REQUIRE(g.num_unflipped_hexes() == 0);
        REQUIRE(!g.node(0));
    }

    SECTION("1 ocean, no edges")
    {
        auto g = Graph({ { 0, NodeType::Ocean } }, {});
        REQUIRE(g.num_hexes() == 0);
        REQUIRE(g.num_junctions() == 0);
        REQUIRE(g.num_oceans() == 1);
        REQUIRE(g.num_roads() == 0);
        REQUIRE(g.num_unflipped_hexes() == 0);
        auto n = g.node(0);
        REQUIRE(n);
        REQUIRE(n->index() == 0);
        REQUIRE(n->type() == NodeType::Ocean);
    }

    SECTION("Duplicate indices should throw")
    {
        REQUIRE_THROWS_AS(Graph({ { 0, NodeType::Ocean }, { 0, NodeType::Ocean } }, {}),
                          std::invalid_argument);
    }

    SECTION("Edges without nodes should throw")
    {
        REQUIRE_THROWS_AS(Graph({}, { { 0, 1, Direction::Clock2 } }), std::invalid_argument);
    }

    SECTION("Multiple edges in a Direction should throw")
    {
        REQUIRE_THROWS_AS(
            Graph({ { 0, NodeType::Ocean }, { 1, NodeType::Junction }, { 2, NodeType::Road } },
                  { { 0, 1, Direction::Clock2 }, { 0, 2, Direction::Clock2 } }),
            std::invalid_argument);
    }

    SECTION("2 nodes, 1 edge")
    {
        auto g = Graph({ { 0, NodeType::Ocean }, { 1, NodeType::Junction } },
                       { { 0, 1, Direction::Clock2 } });
        REQUIRE(g.num_hexes() == 0);
        REQUIRE(g.num_junctions() == 1);
        REQUIRE(g.num_oceans() == 1);
        REQUIRE(g.num_roads() == 0);
        REQUIRE(g.num_unflipped_hexes() == 0);
        auto n0 = g.node(0);
        REQUIRE(n0);
        REQUIRE(n0->index() == 0);
        REQUIRE(n0->type() == NodeType::Ocean);
        auto n1 = g.node(1);
        REQUIRE(n1);
        REQUIRE(n1->index() == 1);
        REQUIRE(n1->type() == NodeType::Junction);
    }
}
