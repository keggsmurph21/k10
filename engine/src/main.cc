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
    for (const auto player : g->players()) {
        const auto actions = player->get_available_actions();
        std::cout << *player << " actions(" << actions.size() << "):" << std::endl;
        for (const auto& available_action : actions) {
            std::cout << " - " << available_action << std::endl;
        }
    }
    std::cout << std::endl;
}

k10engine::Game::Result
build(k10engine::Game::Game* g, size_t player_id, k10engine::Building b, size_t node_id)
{
    return g->execute_action(
        player_id,
        { k10engine::State::Edge::Build,
          { { k10engine::Game::ActionArgumentType::BuildItemId, static_cast<size_t>(b) },
            { k10engine::Game::ActionArgumentType::NodeId, node_id } } });
}

int main(int /* unused */, char** /* unused */)
{
    auto b = k10engine::Board::from_file("static/boards/Standard.board");
    auto s = get_standard_scenario();
    auto p = get_standard_parameters();
    auto g = k10engine::Game::initialize(&b, s, p);

    std::cout << std::endl << " ~~~ Hex neighbors ~~~" << std::endl << std::endl;
    for (const auto& hex_it : g->hexes()) {
        const auto hex = hex_it.second;
        std::cout << *hex << std::endl;
        for (const auto& junction_neighbor : hex->junction_neighbors()) {
            const auto direction = junction_neighbor.first;
            const auto junction = junction_neighbor.second;
            std::cout << " > neighbor in " << direction << " is " << *junction << std::endl;
        }
    }
    std::cout << std::endl << " ~~~ Junction neighbors ~~~" << std::endl << std::endl;
    for (const auto& junction_it : g->junctions()) {
        const auto junction = junction_it.second;
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
    for (const auto& road_it : g->roads()) {
        const auto road = road_it.second;
        std::cout << *road << std::endl;
        for (const auto& junction_neighbor : road->junction_neighbors()) {
            const auto direction = junction_neighbor.first;
            const auto junction = junction_neighbor.second;
            std::cout << " > neighbor in " << direction << " is " << *junction << std::endl;
        }
    }

    dump_actions(g);

    // Round 0, Player 0
    std::cout << build(g, 0, k10engine::Building::Settlement, 1000)
              << std::endl; // Result{ ResultType::NodeIdOutOfRange }
    std::cout << build(g, 0, k10engine::Building::Settlement, 4) << std::endl;
    std::cout << build(g, 0, k10engine::Building::Road, 8) << std::endl;

    // Round 0, Player 1
    std::cout << g->execute_action(1, { k10engine::State::Edge::ToRoot, {} }) << std::endl;
    std::cout << build(g, 1, k10engine::Building::Settlement, 4)
              << std::endl; // Result{ ResultType::NodeIdOutOfRange }
    std::cout << build(g, 1, k10engine::Building::Settlement, 13)
              << std::endl; // Result{ ResultType::NodeIdOutOfRange }
    std::cout << build(g, 1, k10engine::Building::Settlement, 5) << std::endl;
    std::cout << build(g, 1, k10engine::Building::Road, 9) << std::endl;

    // Round 0, Player 2
    std::cout << g->execute_action(2, { k10engine::State::Edge::ToRoot, {} }) << std::endl;
    std::cout << build(g, 2, k10engine::Building::Settlement, 6) << std::endl;
    std::cout << build(g, 2, k10engine::Building::Road, 12) << std::endl;

    // Round 0, Player 3
    std::cout << g->execute_action(3, { k10engine::State::Edge::ToRoot, {} }) << std::endl;
    std::cout << build(g, 3, k10engine::Building::Settlement, 26) << std::endl;
    std::cout << build(g, 3, k10engine::Building::Road, 32) << std::endl;

    // Round 1, Player 3
    std::cout << g->execute_action(3, { k10engine::State::Edge::ToRoot, {} }) << std::endl;
    std::cout << build(g, 3, k10engine::Building::Settlement, 27) << std::endl;
    std::cout << build(g, 3, k10engine::Building::Road, 46) << std::endl;
    std::cout << g->execute_action(3,
                                   { k10engine::State::Edge::ChooseInitialResources,
                                     { { k10engine::Game::ActionArgumentType::NodeId, 27 } } })
              << std::endl;

    dump_actions(g);

    std::cout << g->graph()->width() << "x" << g->graph()->height() << std::endl;
    std::cout << *g->graph()->node(0, 12) << std::endl;

    std::cout << *g << std::endl;

    delete g;

    return 0;
}
