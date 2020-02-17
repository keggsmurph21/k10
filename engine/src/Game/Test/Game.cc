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

namespace k10engine::Game {

using Edge = k10engine::State::Edge;
using Vertex = k10engine::State::Vertex;
using ArgType = k10engine::Game::ActionArgumentType;
using ResType = k10engine::Game::ResultType;
using Building = k10engine::Building;

struct GameChecks {
    bool can_steal;
    bool has_rolled;
    bool is_game_over;
    bool is_trade_accepted;
    bool is_first_round;
    bool is_second_round;
    bool is_roll_seven;
    // bool should_wait_for_discard; // FIXME: Implement
    // bool should_wait_for_trade; // FIXME: Implement
    size_t robber_location;
    size_t dice_total;
    size_t turn;
    size_t round;
    size_t largest_army;
    size_t longest_road;
};

struct PlayerChecks {
    bool can_accept_trade;
    bool has_declined_trade;
    size_t num_to_discard;
    Vertex vertex;
    // bool has_heavy_purse; // FIXME: Implement
    // bool can_build; // FIXME: Implement
    // bool can_play; // FIXME: Implement
    // bool can_trade; // FIXME: Implement
    // bool can_trade_with_bank; // FIXME: Implement
    bool is_current_player;
    // bool is_blocking; // FIXME: Implement
    size_t num_resources;
    size_t army_size;
    size_t longest_road;
    size_t public_victory_points;
    size_t cities;
    size_t roads;
    size_t settlements;
};

void check_game(k10engine::Game::Game* g, GameChecks c)
{
    REQUIRE(g->can_steal() == c.can_steal);
    REQUIRE(g->has_rolled() == c.has_rolled);
    REQUIRE(g->is_game_over() == c.is_game_over);
    REQUIRE(g->is_trade_accepted() == c.is_trade_accepted);
    REQUIRE(g->is_first_round() == c.is_first_round);
    REQUIRE(g->is_second_round() == c.is_second_round);
    REQUIRE(g->is_roll_seven() == c.is_roll_seven);
    // REQUIRE(g->should_wait_for_discard() == c.should_wait_for_discard);
    // REQUIRE(g->should_wait_for_trade() == c.should_wait_for_trade);
    REQUIRE(g->robber_location()->node()->index() == c.robber_location);
    REQUIRE(g->get_dice_total() == c.dice_total);
    REQUIRE(g->turn() == c.turn);
    REQUIRE(g->get_round() == c.round);
    REQUIRE(g->largest_army() == c.largest_army);
    REQUIRE(g->longest_road() == c.longest_road);
}

void check_player(k10engine::Game::Player* p, PlayerChecks c)
{
    REQUIRE(p->can_accept_trade() == c.can_accept_trade);
    REQUIRE(p->has_declined_trade() == c.has_declined_trade);
    REQUIRE(p->num_to_discard() == c.num_to_discard);
    REQUIRE(p->vertex() == c.vertex);
    // REQUIRE(p->has_heavy_purse() == c.has_heavy_purse);
    // REQUIRE(p->can_build() == c.can_build);
    // REQUIRE(p->can_play() == c.can_play);
    // REQUIRE(p->can_trade() == c.can_trade);
    // REQUIRE(p->can_trade_with_bank() == c.can_trade_with_bank);
    REQUIRE(p->is_current_player() == c.is_current_player);
    // REQUIRE(p->is_blocking() == c.is_blocking);
    REQUIRE(p->num_resources() == c.num_resources);
    REQUIRE(p->army_size() == c.army_size);
    REQUIRE(p->longest_road() == c.longest_road);
    REQUIRE(p->public_victory_points() == c.public_victory_points);
    REQUIRE(p->cities().size() == c.cities);           // NOLINT(readability-container-size-empty)
    REQUIRE(p->roads().size() == c.roads);             // NOLINT(readability-container-size-empty)
    REQUIRE(p->settlements().size() == c.settlements); // NOLINT(readability-container-size-empty)
}

TEST_CASE("Game initialization", "[Game]")
{
    SECTION("Single")
    {
        auto b = k10engine::Board::get_single_board();
        auto s = get_single_scenario();
        auto p = get_single_parameters();
        auto g = k10engine::Game::initialize(b, s, p);

        REQUIRE(g->hexes().size() == 1);
        REQUIRE(g->junctions().size() == 6);
        REQUIRE(g->roads().size() == 6);
        REQUIRE(g->players().size() == 1);

        check_game(g, { false, false, false, false, true, false, false, 9, 0, 0, 0, 2, 4 });
        check_player(g->players().at(0),
                     { false, false, 0, Vertex::Root, true, 0, 0, 0, 0, 0, 0, 0 });

        std::vector<k10engine::Game::Action> actions;
        Result r;

        actions = g->players().at(0)->get_available_actions();
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

        r = g->execute_action(1, { Edge::AcceptTrade, {} });
        REQUIRE(r.type == ResType::InvalidPlayerId);
        r = g->execute_action(0, { Edge::AcceptTrade, {} });
        REQUIRE(r.type == ResType::InvalidEdgeChoice);
        r = g->execute_action(0, { Edge::Build, {} });
        REQUIRE(r.type == ResType::InvalidNumberOfArgs);
        r = g->execute_action(0,
                              { Edge::Build, { { ArgType::NodeId, 1 }, { ArgType::NodeId, 1 } } });
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

        r = g->execute_action(
            0,
            { Edge::Build,
              { { ArgType::BuildItemId, static_cast<size_t>(Building::Settlement) },
                { ArgType::NodeId, 2 } } });
        REQUIRE(r.type == ResType::Ok);

        // NB: Can't test ::JunctionNotSettleable yet

        check_game(g, { false, false, false, false, true, false, false, 9, 0, 0, 0, 2, 4 });
        check_player(
            g->players().at(0),
            { false, false, 0, Vertex::AfterBuildingFreeSettlement, true, 0, 0, 0, 1, 0, 0, 1 });

        for (const auto& j_entry : g->junctions()) {
            const auto& j = j_entry.second;
            REQUIRE(j->is_settleable() == (j->index() > 6));
            REQUIRE(j->owner() == (j->index() == 2 ? g->players().at(0) : nullptr));
        }

        actions = g->players().at(0)->get_available_actions();

        REQUIRE(actions.size() == 2);
        for (const auto& action : actions) {
            REQUIRE(action.edge == Edge::Build);
            REQUIRE(action.args.size() == 2);
            REQUIRE(action.args.at(0).type == ArgType::BuildItemId);
            REQUIRE(action.args.at(0).value == static_cast<size_t>(Building::Road));
            REQUIRE(action.args.at(1).type == ArgType::NodeId);
            const auto node_id = action.args.at(1).value;
            REQUIRE(g->roads().find(node_id) != g->roads().end());
            REQUIRE(g->roads().at(node_id)->owner() == nullptr);
        }

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
              { { ArgType::BuildItemId, static_cast<size_t>(Building::Road) },
                { ArgType::BuildItemId, static_cast<size_t>(Building::Road) } } });
        REQUIRE(r.type == ResType::InvalidArgumentType);
        r = g->execute_action(0,
                              { Edge::Build,
                                { { ArgType::BuildItemId, static_cast<size_t>(Building::Road) },
                                  { ArgType::NodeId, 1000 } } });
        REQUIRE(r.type == ResType::NodeIdOutOfRange);
        r = g->execute_action(0,
                              { Edge::Build,
                                { { ArgType::BuildItemId, static_cast<size_t>(Building::Road) },
                                  { ArgType::NodeId, 1 } } });
        REQUIRE(r.type == ResType::InvalidNodeId);

        r = g->execute_action(0,
                              { Edge::Build,
                                { { ArgType::BuildItemId, static_cast<size_t>(Building::Road) },
                                  { ArgType::NodeId, 4 } } });
        REQUIRE(r.type == ResType::Ok);

        check_game(g, { false, false, false, false, false, true, false, 9, 0, 1, 1, 2, 4 });
        check_player(g->players().at(0),
                     { false, false, 0, Vertex::WaitForTurn, true, 0, 0, 0, 1, 0, 1, 1 });

        actions = g->players().at(0)->get_available_actions();

        REQUIRE(actions.size() == 1);
        for (const auto& action : actions) {
            REQUIRE(action.edge == Edge::ToRoot);
            REQUIRE(action.args.size() == 0); // NOLINT(readability-container-size-empty)
        }

        r = g->execute_action(0, { Edge::ToRoot, {} });

        REQUIRE(r.type == ResType::Ok);

        check_game(g, { false, false, false, false, false, true, false, 9, 0, 1, 1, 2, 4 });
        check_player(g->players().at(0),
                     { false, false, 0, Vertex::Root, true, 0, 0, 0, 1, 0, 1, 1 });

        actions = g->players().at(0)->get_available_actions();

        REQUIRE(actions.size() == 3);
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

        r = g->execute_action(
            0,
            { Edge::Build,
              { { ArgType::BuildItemId, static_cast<size_t>(Building::Settlement) },
                { ArgType::NodeId, 5 } } });
        REQUIRE(r.type == ResType::JunctionNotSettleable);

        r = g->execute_action(
            0,
            { Edge::Build,
              { { ArgType::BuildItemId, static_cast<size_t>(Building::Settlement) },
                { ArgType::NodeId, 6 } } });
        REQUIRE(r.type == ResType::JunctionNotSettleable);

        r = g->execute_action(
            0,
            { Edge::Build,
              { { ArgType::BuildItemId, static_cast<size_t>(Building::Settlement) },
                { ArgType::NodeId, 12 } } });
        REQUIRE(r.type == ResType::Ok);

        check_game(g, { false, false, false, false, false, true, false, 9, 0, 1, 1, 2, 4 });
        check_player(
            g->players().at(0),
            { false, false, 0, Vertex::AfterBuildingFreeSettlement, true, 0, 0, 0, 2, 0, 1, 2 });

        actions = g->players().at(0)->get_available_actions();

        REQUIRE(actions.size() == 4);
        for (const auto& action : actions) {
            REQUIRE(action.edge == Edge::Build);
            REQUIRE(action.args.size() == 2);
            REQUIRE(action.args.at(0).type == ArgType::BuildItemId);
            REQUIRE(action.args.at(0).value == static_cast<size_t>(Building::Road));
            REQUIRE(action.args.at(1).type == ArgType::NodeId);
            const auto node_id = action.args.at(1).value;
            REQUIRE(g->roads().find(node_id) != g->roads().end());
            REQUIRE(g->roads().at(node_id)->owner() == nullptr);
        }

        r = g->execute_action(
            0,
            { Edge::Build,
              { { ArgType::BuildItemId, static_cast<size_t>(Building::Settlement) },
                { ArgType::NodeId, 13 } } });
        REQUIRE(r.type == ResType::InvalidEdgeChoice);

        r = g->execute_action(0,
                              { Edge::Build,
                                { { ArgType::BuildItemId, static_cast<size_t>(Building::Road) },
                                  { ArgType::NodeId, 10 } } });
        REQUIRE(r.type == ResType::Ok);

        delete g;
        delete b;
    }
}

} // namespace k10engine::Game
