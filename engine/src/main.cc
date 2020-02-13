#include <iostream>

#include "Board/generated/Standard.h"
#include "Game/Game.h"
#include "Scenario/Scenario.h"

int main(int /* unused */, char** /* unused */)
{
    auto b = k10engine::Board::get_standard_board();

    auto s = k10engine::Scenario::Scenario(
        2,  // NOLINT(readability-magic-numbers)
        5,  // NOLINT(readability-magic-numbers)
        8,  // NOLINT(readability-magic-numbers)
        12, // NOLINT(readability-magic-numbers)
        { {
              k10engine::Building::City,
              { { k10engine::Resource::Ore, 3 }, { k10engine::Resource::Wheat, 2 } },
          },
          {
              k10engine::Building::DevelopmentCard,
              { { k10engine::Resource::Ore, 1 },
                { k10engine::Resource::Sheep, 1 },
                { k10engine::Resource::Wheat, 1 } },
          },
          {
              k10engine::Building::Road,
              {
                  { k10engine::Resource::Brick, 1 },
                  { k10engine::Resource::Wood, 1 },
              },
          },
          {
              k10engine::Building::Settlement,
              { { k10engine::Resource::Brick, 1 },
                { k10engine::Resource::Sheep, 1 },
                { k10engine::Resource::Wheat, 1 },
                { k10engine::Resource::Wood, 1 } },
          } },
        {
            { k10engine::Building::City, 4 },
            { k10engine::Building::DevelopmentCard, 25 }, // NOLINT(readability-magic-numbers)
            { k10engine::Building::Road, 15 },            // NOLINT(readability-magic-numbers)
            { k10engine::Building::Settlement, 5 },       // NOLINT(readability-magic-numbers)
        },
        {
            { k10engine::DevelopmentCard::Knight, 14 }, // NOLINT(readability-magic-numbers)
            { k10engine::DevelopmentCard::Monopoly, 2 },
            { k10engine::DevelopmentCard::RoadBuilding, 2 },
            { k10engine::DevelopmentCard::VictoryPoint, 5 }, // NOLINT(readability-magic-numbers)
            { k10engine::DevelopmentCard::YearOfPlenty, 2 },
        },
        { { k10engine::Resource::Brick, 3 },
          { k10engine::Resource::Ore, 3 },
          { k10engine::Resource::Sheep, 4 },
          { k10engine::Resource::Wheat, 4 },
          { k10engine::Resource::Wood, 4 },
          { k10engine::NonYieldingResource::Desert, 1 } },
        // NOLINTNEXTLINE(readability-magic-numbers)
        { 2, 3, 3, 4, 4, 5, 5, 6, 6, 8, 8, 9, 9, 10, 10, 11, 11, 12 },
        {
            { { k10engine::Resource::Brick,
                k10engine::Resource::Ore,
                k10engine::Resource::Sheep,
                k10engine::Resource::Wheat,
                k10engine::Resource::Wood },
              4 },
            { { k10engine::Resource::Wheat }, 3 },
            { {
                  k10engine::Resource::Ore,
              },
              3 },
            { { k10engine::Resource::Brick,
                k10engine::Resource::Ore,
                k10engine::Resource::Sheep,
                k10engine::Resource::Wheat,
                k10engine::Resource::Wood },
              4 },
            { { k10engine::Resource::Brick,
                k10engine::Resource::Ore,
                k10engine::Resource::Sheep,
                k10engine::Resource::Wheat,
                k10engine::Resource::Wood },
              4 },
            { {
                  k10engine::Resource::Sheep,
              },
              3 },
            { { k10engine::Resource::Brick,
                k10engine::Resource::Ore,
                k10engine::Resource::Sheep,
                k10engine::Resource::Wheat,
                k10engine::Resource::Wood },
              4 },
            { {
                  k10engine::Resource::Brick,
              },
              3 },
            { {
                  k10engine::Resource::Wood,
              },
              3 },
        });
    auto p = k10engine::Scenario::Parameters{ k10engine::Scenario::IterationType::Fixed,
                                              k10engine::Scenario::IterationType::Fixed,
                                              k10engine::Scenario::IterationType::Fixed,
                                              k10engine::Scenario::IterationType::Fixed,
                                              4,    // NOLINT(readability-magic-numbers)
                                              10 }; // NOLINT(readability-magic-numbers)
    auto g = k10engine::Game::initialize(b, s, p);

    for (const auto& hex : g->hexes()) {
        std::cout << *hex << std::endl;
    }
    for (const auto& junction : g->junctions()) {
        std::cout << *junction << std::endl;
    }
    for (const auto& road : g->roads()) {
        std::cout << *road << std::endl;
    }

    delete g;
    delete b;

    return 0;
}
