#include "Board/Parser.h"
#include "Test/catch.h"

namespace k10engine::Board {

TEST_CASE("Parse string", "[Board][Board.Parser]")
{
    SECTION("Standard")
    {
        k10engine::Board::from_string(R"(
            o       o       o       o
                p       p       .
              /   \   /   \   /   \
            p       .       p       .
        o   |   h   |   h   |   h   |   o
            .       .       .       p
          /   \   /   \   /   \   /   \
        p       .       .       .       p
    o   |   h   |   h   |   h   |   h   |   o
        p       .       .       .       .
      /   \   /   \   /   \   /   \   /   \
    .       .       .       .       .       p
o   |   h   |   h   |   h   |   h   |   h   |   o
    .       .       .       .       .       p
      \   /   \   /   \   /   \   /   \   /
        p       .       .       .       .
    o   |   h   |   h   |   h   |   h   |   o
        p       .       .       .       p
          \   /   \   /   \   /   \   /
            .       .       .       p
        o   |   h   |   h   |   h   |   o
            p       .       p       .
              \   /   \   /   \   /
                p       p       .
            o       o       o       o
    )");
    }
}

TEST_CASE("Parse file", "[Board][Board.Parser]")
{
    SECTION("Portless") { k10engine::Board::from_file("static/boards/Portless.board"); }
    SECTION("Single") { k10engine::Board::from_file("static/boards/Single.board"); }
    SECTION("Standard") { k10engine::Board::from_file("static/boards/Standard.board"); }
    SECTION("Tall") { k10engine::Board::from_file("static/boards/Tall.board"); }
    SECTION("Triple") { k10engine::Board::from_file("static/boards/Triple.board"); }
}

} // namespace k10engine::Board
