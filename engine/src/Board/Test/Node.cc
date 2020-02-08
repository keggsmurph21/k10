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
    }
}
