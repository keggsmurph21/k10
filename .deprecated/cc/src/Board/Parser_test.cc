#include "Board/Parser.h"
#include "Test/catch.h"

namespace k10engine::Board {

TEST_CASE("Parse string", "[Board][Board.Parser]")
{
    SECTION("Standard")
    {
        const auto* b = k10engine::Board::from_string(R"(
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
        o   |   h   |   h   |   h   |   oo
            p       .       p       .
              \   /   \   /   \   /
                p       p       .
            o       o       o       o
    )");
        delete b;
    }
}

TEST_CASE("Parse file", "[Board][Board.Parser]")
{
    SECTION("Portless")
    {
        const auto* b = k10engine::Board::from_file("static/Portless.board");
        REQUIRE(b->size() > 0);
        delete b;
    }
    SECTION("Single")
    {
        const auto* b = k10engine::Board::from_file("static/Single.board");
        REQUIRE(b->size() > 0);
        delete b;
    }
    SECTION("Standard")
    {
        const auto* b = k10engine::Board::from_file("static/Standard.board");
        REQUIRE(b->size() > 0);
        delete b;
    }
    SECTION("Tall")
    {
        const auto* b = k10engine::Board::from_file("static/Tall.board");
        REQUIRE(b->size() > 0);
        delete b;
    }
    SECTION("Triple")
    {
        const auto* b = k10engine::Board::from_file("static/Triple.board");
        REQUIRE(b->size() > 0);
        delete b;
    }
}

} // namespace k10engine::Board
