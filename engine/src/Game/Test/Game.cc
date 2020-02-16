#include <iostream>
#include <stdexcept>
#include <vector>

#include "Board/generated/Single.h"
#include "Board/generated/Triple.h"
#include "Game/Game.h"
#include "Scenario/Parameters.h"
#include "Scenario/Scenario.h"
#include "Test/catch.h"

k10engine::Scenario::Scenario get_single_scenario()
{
    return k10engine::Scenario::Scenario(
        1, // NOLINT(readability-magic-numbers)
        2, // NOLINT(readability-magic-numbers)
        3, // NOLINT(readability-magic-numbers)
        6, // NOLINT(readability-magic-numbers)
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
            { k10engine::Building::City, 2 },
            { k10engine::Building::DevelopmentCard, 25 }, // NOLINT(readability-magic-numbers)
            { k10engine::Building::Road, 6 },             // NOLINT(readability-magic-numbers)
            { k10engine::Building::Settlement, 2 },       // NOLINT(readability-magic-numbers)
        },
        {
            { k10engine::DevelopmentCard::Knight, 14 }, // NOLINT(readability-magic-numbers)
            { k10engine::DevelopmentCard::Monopoly, 2 },
            { k10engine::DevelopmentCard::RoadBuilding, 2 },
            { k10engine::DevelopmentCard::VictoryPoint, 5 }, // NOLINT(readability-magic-numbers)
            { k10engine::DevelopmentCard::YearOfPlenty, 2 },
        },
        {
            { k10engine::NonYieldingResource::Desert, 1 },
        },
        {},
        {
            { { k10engine::Resource::Brick,
                k10engine::Resource::Ore,
                k10engine::Resource::Sheep,
                k10engine::Resource::Wheat,
                k10engine::Resource::Wood },
              4 },
            { { k10engine::Resource::Wheat }, 3 },
        });
}

k10engine::Scenario::Parameters get_single_parameters()
{
    return k10engine::Scenario::Parameters{ k10engine::Scenario::IterationType::Fixed,
                                            k10engine::Scenario::IterationType::Fixed,
                                            k10engine::Scenario::IterationType::Fixed,
                                            k10engine::Scenario::IterationType::Fixed,
                                            1,   // NOLINT(readability-magic-numbers)
                                            3 }; // NOLINT(readability-magic-numbers)
}

k10engine::Game::Game* get_single()
{
    auto b = k10engine::Board::get_single_board();
    auto s = get_single_scenario();
    auto p = get_single_parameters();
    return k10engine::Game::initialize(b, s, p);
}

namespace k10engine::Game {

using Edge = k10engine::State::Edge;
using ArgType = k10engine::Game::ActionArgumentType;
using ResType = k10engine::Game::ResultType;
using Building = k10engine::Building;

TEST_CASE("Game initialization", "[Game]")
{
    SECTION("Single")
    {
        auto g = get_single();
        const auto& actions = g->players().at(0)->get_available_actions();
        REQUIRE(actions.size() == 6);
        for (const auto& action : actions) {
            REQUIRE(action.edge == Edge::Build);
            REQUIRE(action.args.size() == 2);
            REQUIRE(action.args.at(0).type == ArgType::BuildItemId);
            REQUIRE(action.args.at(0).value == static_cast<size_t>(Building::Settlement));
            REQUIRE(action.args.at(1).type == ArgType::NodeId);
            const auto node_id = action.args.at(1).value;
            REQUIRE(g->junctions().find(node_id) != g->junctions().end());
            REQUIRE(g->junctions().at(node_id)->is_settleable() == true);
        }
        Result r;
        r = g->execute_action(1, { Edge::AcceptTrade, {} });
        REQUIRE(r.type == ResType::InvalidPlayerId);
        r = g->execute_action(0, { Edge::AcceptTrade, {} });
        REQUIRE(r.type == ResType::InvalidEdgeChoice);
        r = g->execute_action(0, { Edge::Build, {} });
        REQUIRE(r.type == ResType::InvalidNumberOfArgs);
        r = g->execute_action(0,
                              { Edge::Build, { { ArgType::NodeId, 4 }, { ArgType::NodeId, 4 } } });
        REQUIRE(r.type == ResType::InvalidArgumentType);
        r = g->execute_action(
            0,
            { Edge::Build,
              { { ArgType::BuildItemId, static_cast<size_t>(Building::Settlement) },
                { ArgType::BuildItemId, static_cast<size_t>(Building::Settlement) } } });
        REQUIRE(r.type == ResType::InvalidArgumentType);
        r = g->execute_action(
            0,
            { Edge::Build,
              { { ArgType::BuildItemId, static_cast<size_t>(Building::Settlement) },
                { ArgType::NodeId, 1000 } } });
        REQUIRE(r.type == ResType::NodeIdOutOfRange);
        r = g->execute_action(
            0,
            { Edge::Build,
              { { ArgType::BuildItemId, static_cast<size_t>(Building::Settlement) },
                { ArgType::NodeId, 1 } } });
        REQUIRE(r.type == ResType::InvalidNodeId);
        // NB: Can't test ::JunctionNotSettleable yet
    }
}

} // namespace k10engine::Game
