#include <iostream>

#include "Board/Parser.h"
#include "Game/Game.h"
#include "Scenario/Scenario.h"

k10engine::Scenario::Scenario get_standard_scenario()
{
    return k10engine::Scenario::Scenario(
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
            { k10engine::Building::DevelopmentCard, 25 },
        },
        {
            { k10engine::Building::City, 4 },
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
              3 },
            { { k10engine::Resource::Wheat }, 2 },
            { {
                  k10engine::Resource::Ore,
              },
              2 },
            { { k10engine::Resource::Brick,
                k10engine::Resource::Ore,
                k10engine::Resource::Sheep,
                k10engine::Resource::Wheat,
                k10engine::Resource::Wood },
              3 },
            { { k10engine::Resource::Brick,
                k10engine::Resource::Ore,
                k10engine::Resource::Sheep,
                k10engine::Resource::Wheat,
                k10engine::Resource::Wood },
              3 },
            { {
                  k10engine::Resource::Sheep,
              },
              2 },
            { { k10engine::Resource::Brick,
                k10engine::Resource::Ore,
                k10engine::Resource::Sheep,
                k10engine::Resource::Wheat,
                k10engine::Resource::Wood },
              3 },
            { {
                  k10engine::Resource::Brick,
              },
              2 },
            { {
                  k10engine::Resource::Wood,
              },
              2 },
        });
}

k10engine::Scenario::Parameters get_standard_parameters()
{
    return k10engine::Scenario::Parameters{ k10engine::Scenario::IterationType::Fixed,
                                            k10engine::Scenario::IterationType::Fixed,
                                            k10engine::Scenario::IterationType::Fixed,
                                            k10engine::Scenario::IterationType::Fixed,
                                            4,
                                            10 };
}

void dump_actions(const k10engine::Game::Game* g)
{
    std::cout << std::endl;
    for (const auto& player : g->players()) {
        const auto actions = player.get_available_actions();
        std::cout << player << " actions(" << actions.size() << "):" << std::endl;
        for (const auto& available_action : actions) {
            std::cout << " - " << available_action << std::endl;
        }
    }
    std::cout << std::endl;
}

int main(int /* unused */, char** /* unused */)
{
    auto b = k10engine::Board::from_file("static/boards/Standard.board");
    auto s = get_standard_scenario();
    auto p = get_standard_parameters();
    auto g = k10engine::Game::initialize(&b, s, p);

    std::cout << std::endl << " ~~~ Hex neighbors ~~~" << std::endl << std::endl;
    g->for_each_hex([&](const auto* hex) {
        std::cout << *hex << std::endl;
        hex->for_each_junction_neighbor([&](const auto& direction, const auto* junction) {
            std::cout << " > neighbor in " << direction << " is " << *junction << std::endl;
        });
    });

    std::cout << std::endl << " ~~~ Junction neighbors ~~~" << std::endl << std::endl;
    g->for_each_junction([&](const auto* junction) {
        std::cout << *junction << std::endl;
        junction->for_each_hex_neighbor([&](const auto& direction, const auto* hex) {
            std::cout << " > neighbor in " << direction << " is " << *hex << std::endl;
        });
        junction->for_each_road_neighbor([&](const auto& direction, const auto* junction) {
            std::cout << " > neighbor in " << direction << " is " << *junction << std::endl;
        });
    });

    std::cout << std::endl << " ~~~ Road neighbors ~~~" << std::endl << std::endl;
    g->for_each_road([&](const auto* road) {
        std::cout << *road << std::endl;
        road->for_each_junction_neighbor([&](const auto& direction, const auto* junction) {
            std::cout << " > neighbor in " << direction << " is " << *junction << std::endl;
        });
    });

    dump_actions(g);

    // Round 0, Player 0
    std::cout << g->execute_build_settlement(g->player(0), g->junction(4)) << std::endl;
    std::cout << g->execute_build_road(g->player(0), g->road(8)) << std::endl;

    // Round 0, Player 1
    std::cout << g->execute_to_root(g->player(1)) << std::endl;
    std::cout << g->execute_build_settlement(g->player(1), g->junction(5)) << std::endl;
    std::cout << g->execute_build_road(g->player(1), g->road(9)) << std::endl;

    // Round 0, Player 2
    std::cout << g->execute_to_root(g->player(2)) << std::endl;
    std::cout << g->execute_build_settlement(g->player(2), g->junction(6)) << std::endl;
    std::cout << g->execute_build_road(g->player(2), g->road(12)) << std::endl;

    // Round 0, Player 3
    std::cout << g->execute_to_root(g->player(3)) << std::endl;
    std::cout << g->execute_build_settlement(g->player(3), g->junction(26)) << std::endl;
    std::cout << g->execute_build_road(g->player(3), g->road(31)) << std::endl;

    // Round 1, Player 3
    std::cout << g->execute_to_root(g->player(3)) << std::endl;
    std::cout << g->execute_build_settlement(g->player(3), g->junction(27)) << std::endl;
    std::cout << g->execute_build_road(g->player(3), g->road(46)) << std::endl;
    std::cout << g->execute_choose_initial_resources(g->player(3), g->junction(27)) << std::endl;

    dump_actions(g);

    std::cout << g->graph()->width() << "x" << g->graph()->height() << std::endl;
    std::cout << *g->graph()->node(0, 12) << std::endl;

    std::cout << *g << std::endl;

    delete g;

    return 0;
}
