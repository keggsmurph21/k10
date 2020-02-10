#include <iostream>

#include "Board/generated/Standard.h"
#include "Game/Game.h"
#include "Scenario/Scenario.h"

int main(int /* unused */, char** /* unused */)
{
    auto b = k10engine::Board::get_standard_board();
    // NEXTLINENOLINT(readability-magic-numbers)
    auto s = k10engine::Scenario::Scenario(
        2,
        5,
        8,
        12,
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
            { k10engine::Building::DevelopmentCard, 25 },
            { k10engine::Building::Road, 15 },
            { k10engine::Building::Settlement, 5 },
        },
        {
            { k10engine::DevelopmentCard::Knight, 14 },
            { k10engine::DevelopmentCard::Monopoly, 2 },
            { k10engine::DevelopmentCard::RoadBuilding, 2 },
            { k10engine::DevelopmentCard::VictoryPoint, 5 },
            { k10engine::DevelopmentCard::YearOfPlenty, 2 },
        },
        { { k10engine::Resource::Brick, 3 },
          { k10engine::Resource::Ore, 3 },
          { k10engine::Resource::Sheep, 4 },
          { k10engine::Resource::Wheat, 4 },
          { k10engine::Resource::Wood, 4 },
          { k10engine::NonYieldingResource::Desert, 1 } },
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
    // NEXTLINENOLINT(readability-magic-numbers)
    auto p = k10engine::Scenario::Parameters{ k10engine::Scenario::IterationType::Fixed,
                                              k10engine::Scenario::IterationType::Fixed,
                                              k10engine::Scenario::IterationType::Fixed,
                                              k10engine::Scenario::IterationType::Fixed,
                                              4,
                                              10 };
    auto g = k10engine::Game::initialize(b, s, p);
    (void)g;

    delete b;

    return 0;
}
