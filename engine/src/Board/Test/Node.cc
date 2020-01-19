#include "Board/Node.h"
#include "Test/catch.h"

using namespace k10engine::Board;

TEST_CASE("Node initialization", "[Board][Board.Node]")
{
    SECTION("Ocean")
    {
        auto node = Node(0, NodeType::Ocean);
        REQUIRE(node.index() == 0);
        REQUIRE(node.type() == NodeType::Ocean);
        REQUIRE(!node.has_edge(Direction::Clock2));
        REQUIRE(!node.has_edge(Direction::Clock4));
        REQUIRE(!node.has_edge(Direction::Clock6));
        REQUIRE(!node.has_edge(Direction::Clock8));
        REQUIRE(!node.has_edge(Direction::Clock10));
        REQUIRE(!node.has_edge(Direction::Clock12));
        REQUIRE(!node.get_edge(Direction::Clock2));
        REQUIRE(!node.get_edge(Direction::Clock4));
        REQUIRE(!node.get_edge(Direction::Clock6));
        REQUIRE(!node.get_edge(Direction::Clock8));
        REQUIRE(!node.get_edge(Direction::Clock10));
        REQUIRE(!node.get_edge(Direction::Clock12));
        REQUIRE(node.neighbors()->size() == 0);
    }
}

TEST_CASE("Node neighbors", "[Board][Board.Node]")
{
    SECTION("Ocean + Junc")
    {
        auto n0 = new Node(0, NodeType::Ocean);
        auto n1 = new Node(1, NodeType::Junction);
        n0->add_edge(Direction::Clock2, n1);
        REQUIRE(n0->has_edge(Direction::Clock2));
        REQUIRE(n0->get_edge(Direction::Clock2) == n1);
        REQUIRE(n0->neighbors()->size() == 1);
        REQUIRE(!n1->has_edge(Direction::Clock8));
        REQUIRE(!n1->get_edge(Direction::Clock8));
        REQUIRE(n1->neighbors()->size() == 0);
        n1->add_edge(Direction::Clock8, n0);
        REQUIRE(n1->has_edge(Direction::Clock8));
        REQUIRE(n1->get_edge(Direction::Clock8) == n0);
        REQUIRE(n1->neighbors()->size() == 1);
        REQUIRE(n0->has_edge(Direction::Clock2));
        REQUIRE(n0->get_edge(Direction::Clock2));
        REQUIRE(n0->neighbors()->size() == 1);
        delete n0;
        delete n1;
    }
}
