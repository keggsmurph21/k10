#include <stdexcept>

#include "Board/Port.h"
#include "Test/catch.h"

using namespace k10engine::Board;

TEST_CASE("Port initialization", "[Board][Board.Port]")
{
    auto o = Orientation::Clock2Clock8;

    SECTION("Constructing from nullptr's should throw")
    {
        auto n = Node(0, NodeType::Junction);
        REQUIRE_THROWS_AS(Port({ nullptr, nullptr }, o), std::invalid_argument);
        REQUIRE_THROWS_AS(Port({ &n, nullptr }, o), std::invalid_argument);
        REQUIRE_THROWS_AS(Port({ nullptr, &n }, o), std::invalid_argument);
    }

    SECTION("Invalid NodeTypes should throw")
    {
        auto nodes = std::set<Node>{
            Node(0, NodeType::Hex),  Node(1, NodeType::Ocean),        Node(2, NodeType::Junction),
            Node(3, NodeType::Road), Node(4, NodeType::UnflippedHex),
        };
        for (auto n0 : nodes) {
            for (auto n1 : nodes) {
                if (n0.type() != NodeType::Junction && n1.type() != NodeType::Junction)
                    REQUIRE_THROWS_AS(Port({ &n0, &n1 }, o), std::invalid_argument);
            }
        }
    }

    SECTION("Port initialization")
    {
        auto n0 = Node(0, NodeType::Junction);
        auto n1 = Node(1, NodeType::Junction);
        // FIXME: The following call should throw if made in the Board ctor
        auto p = Port({ &n0, &n1 }, o);
        REQUIRE(p.orientation() == o);
        REQUIRE(p.nodes().size() == 2);
        auto n = p.nodes().begin();
        REQUIRE(*n == const_cast<const Node*>(&n0));
        ++n;
        REQUIRE(*n == const_cast<const Node*>(&n1));
    }
}
