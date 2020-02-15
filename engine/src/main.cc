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

    std::cout << std::endl << " ~~~ Hex neighbors ~~~" << std::endl << std::endl;
    for (const auto& hex_entry : g->hexes()) {
        const auto hex = hex_entry.second;
        std::cout << *hex << std::endl;
        for (const auto& junction_neighbor : hex->junction_neighbors()) {
            const auto direction = junction_neighbor.first;
            const auto junction = junction_neighbor.second;
            std::cout << " > neighbor in " << direction << " is " << *junction << std::endl;
        }
    }
    std::cout << std::endl << " ~~~ Junction neighbors ~~~" << std::endl << std::endl;
    for (const auto& junction_entry : g->junctions()) {
        const auto junction = junction_entry.second;
        std::cout << *junction << std::endl;
        for (const auto& hex_neighbor : junction->hex_neighbors()) {
            const auto direction = hex_neighbor.first;
            const auto hex = hex_neighbor.second;
            std::cout << " > neighbor in " << direction << " is " << *hex << std::endl;
        }
        for (const auto& road_neighbor : junction->road_neighbors()) {
            const auto direction = road_neighbor.first;
            const auto road = road_neighbor.second;
            std::cout << " > neighbor in " << direction << " is " << *road << std::endl;
        }
    }
    std::cout << std::endl << " ~~~ Road neighbors ~~~" << std::endl << std::endl;
    for (const auto& road_entry : g->roads()) {
        const auto road = road_entry.second;
        std::cout << *road << std::endl;
        for (const auto& junction_neighbor : road->junction_neighbors()) {
            const auto direction = junction_neighbor.first;
            const auto junction = junction_neighbor.second;
            std::cout << " > neighbor in " << direction << " is " << *junction << std::endl;
        }
    }

    for (const auto player : g->players()) {
        const auto actions = player->get_available_actions();
        std::cout << *player << " actions(" << actions.size() << "):" << std::endl;
        for (const auto& available_action : actions) {
            std::cout << " - " << available_action << std::endl;
        }
    }

    g->execute_action(0,
                      { k10engine::State::Edge::Build,
                        { { k10engine::Game::ActionArgumentType::BuildItemId,
                            static_cast<size_t>(k10engine::Building::Settlement) },
                          { k10engine::Game::ActionArgumentType::NodeId, 4 } } });

    delete g;
    delete b;

    return 0;
}
