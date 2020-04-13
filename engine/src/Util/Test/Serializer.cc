#include <iostream>

#include "Game/BoardView/Hex.h"
#include "Game/BoardView/Junction.h"
#include "Game/BoardView/Road.h"
#include "Scenario/Scenario.h"
#include "Test/catch.h"

namespace k10engine {

TEST_CASE("Serialize Scenario", "[Test][Test.Serializer][Test.Serializer.Scenario]")
{
    Scenario::Scenario s{
        1,
        2,
        3,
        6,
        { {
              Building::City,
              { { Resource::Ore, 3 }, { Resource::Wheat, 2 } },
          },
          {
              Building::DevelopmentCard,
              { { Resource::Ore, 1 }, { Resource::Sheep, 1 }, { Resource::Wheat, 1 } },
          },
          {
              Building::Road,
              {
                  { Resource::Brick, 1 },
                  { Resource::Wood, 1 },
              },
          },
          {
              Building::Settlement,
              { { Resource::Brick, 1 },
                { Resource::Sheep, 1 },
                { Resource::Wheat, 1 },
                { Resource::Wood, 1 } },
          } },
        {
            { Building::DevelopmentCard, 25 },
        },
        {
            { Building::City, 2 },
            { Building::Road, 6 },
            { Building::Settlement, 2 },
        },
        {
            { DevelopmentCard::Knight, 14 },
            { DevelopmentCard::Monopoly, 2 },
            { DevelopmentCard::RoadBuilding, 2 },
            { DevelopmentCard::VictoryPoint, 5 },
            { DevelopmentCard::YearOfPlenty, 2 },
        },
        {
            { NonYieldingResource::Desert, 1 },
        },
        {},
        {
            { { Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat, Resource::Wood },
              3 },
            { { Resource::Wheat }, 2 },
        }
    };

    /*
    for (const auto& ch : s.serialize()) {
        std::cout << ch;
    }
    std::cout << std::endl;
    */

    REQUIRE(s == Scenario::Scenario::deserialize(s.serialize()));
}

TEST_CASE("Serialize NodeView", "[Test][Test.Serializer][Test.Serializer.NodeView]")
{
    SECTION("Serialize Hex")
    {
        const Board::Node node{ 0, 0, 0, Board::NodeType::Hex };
        Game::BoardView::Hex h{ node, Resource::Brick, 2 };
        /*
        for (const auto& ch : h.serialize()) {
            std::cout << ch;
        }
        std::cout << std::endl;
        */
        REQUIRE(h == Game::BoardView::Hex::deserialize(node, h.serialize()));
    }

    SECTION("Serialize Junction")
    {
        const Board::Node node{ 0, 0, 0, Board::NodeType::Junction };
        Game::BoardView::Junction j{ node, { Resource::Brick, Resource::Wood }, 5 };
        /*
        for (const auto& ch : j.serialize()) {
            std::cout << ch;
        }
        std::cout << std::endl;
        */
        REQUIRE(j == Game::BoardView::Junction::deserialize(node, j.serialize()));
    }

    SECTION("Serialize Road")
    {
        const Board::Node node{ 0, 0, 0, Board::NodeType::Road };
        Game::BoardView::Road r{ node };
        /*
        for (const auto& ch : r.serialize()) {
            std::cout << ch;
        }
        std::cout << std::endl;
        */
        REQUIRE(r == Game::BoardView::Road::deserialize(node, r.serialize()));
    }
}

} // namespace k10engine
