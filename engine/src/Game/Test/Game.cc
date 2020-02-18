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

k10engine::Scenario::Scenario get_triple_scenario()
{
    return k10engine::Scenario::Scenario(
        1, // NOLINT(readability-magic-numbers)
        3, // NOLINT(readability-magic-numbers)
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
            { k10engine::Building::City, 3 },
            { k10engine::Building::DevelopmentCard, 25 }, // NOLINT(readability-magic-numbers)
            { k10engine::Building::Road, 10 },            // NOLINT(readability-magic-numbers)
            { k10engine::Building::Settlement, 4 },       // NOLINT(readability-magic-numbers)
        },
        {
            { k10engine::DevelopmentCard::Knight, 14 }, // NOLINT(readability-magic-numbers)
            { k10engine::DevelopmentCard::Monopoly, 2 },
            { k10engine::DevelopmentCard::RoadBuilding, 2 },
            { k10engine::DevelopmentCard::VictoryPoint, 5 }, // NOLINT(readability-magic-numbers)
            { k10engine::DevelopmentCard::YearOfPlenty, 2 },
        },
        {
            { k10engine::Resource::Brick, 1 },
            { k10engine::Resource::Wood, 1 },
            { k10engine::NonYieldingResource::Desert, 1 },
        },
        { 6, 9 },
        {
            { { k10engine::Resource::Brick }, 3 },
            { { k10engine::Resource::Wood }, 3 },
            { { k10engine::Resource::Wheat }, 3 },
            { { k10engine::Resource::Brick,
                k10engine::Resource::Ore,
                k10engine::Resource::Sheep,
                k10engine::Resource::Wheat,
                k10engine::Resource::Wood },
              4 },
        });
}

k10engine::Scenario::Parameters get_triple_parameters(int num_players)
{
    return k10engine::Scenario::Parameters{ k10engine::Scenario::IterationType::Fixed,
                                            k10engine::Scenario::IterationType::Fixed,
                                            k10engine::Scenario::IterationType::Fixed,
                                            k10engine::Scenario::IterationType::Fixed,
                                            num_players, // NOLINT(readability-magic-numbers)
                                            3 };         // NOLINT(readability-magic-numbers)
}

namespace k10engine::Game {

using Edge = k10engine::State::Edge;
using Vertex = k10engine::State::Vertex;
using Action = k10engine::Game::Action;
using ArgType = k10engine::Game::ActionArgumentType;
using ResType = k10engine::Game::ResultType;
using Building = k10engine::Building;

struct GameState {
    bool can_steal = false;
    bool has_rolled = false;
    bool is_game_over = false;
    bool is_trade_accepted = false;
    bool is_first_round = true;
    bool is_second_round = false;
    bool is_roll_seven = false;
    // bool should_wait_for_discard = false; // FIXME: Implement
    // bool should_wait_for_trade = false; // FIXME: Implement
    size_t robber_location = 0;
    size_t dice_total = 0;
    size_t turn = 0;
    size_t round = 0;
    size_t largest_army = 2;
    size_t longest_road = 4;
};

struct PlayerState {
    bool can_accept_trade = false;
    bool has_declined_trade = false;
    size_t num_to_discard = 0;
    Vertex vertex = Vertex::Root;
    // bool has_heavy_purse = false; // FIXME: Implement
    // bool can_build = false; // FIXME: Implement
    // bool can_play = false; // FIXME: Implement
    // bool can_trade = false; // FIXME: Implement
    // bool can_trade_with_bank = false; // FIXME: Implement
    bool is_current_player = false;
    // bool is_blocking = false; // FIXME: Implement
    size_t num_resources = 0;
    size_t army_size = 0;
    size_t longest_road = 0;
    size_t public_victory_points = 0;
    size_t cities = 0;
    size_t roads = 0;
    size_t settlements = 0;
};

void check_game(k10engine::Game::Game* g, GameState gs)
{
    REQUIRE(g->can_steal() == gs.can_steal);
    REQUIRE(g->has_rolled() == gs.has_rolled);
    REQUIRE(g->is_game_over() == gs.is_game_over);
    REQUIRE(g->is_trade_accepted() == gs.is_trade_accepted);
    REQUIRE(g->is_first_round() == gs.is_first_round);
    REQUIRE(g->is_second_round() == gs.is_second_round);
    REQUIRE(g->is_roll_seven() == gs.is_roll_seven);
    // REQUIRE(g->should_wait_for_discard() == gs.should_wait_for_discard);
    // REQUIRE(g->should_wait_for_trade() == gs.should_wait_for_trade);
    REQUIRE(g->robber_location()->node()->index() == gs.robber_location);
    REQUIRE(g->get_dice_total() == gs.dice_total);
    REQUIRE(g->turn() == gs.turn);
    REQUIRE(g->get_round() == gs.round);
    REQUIRE(g->largest_army() == gs.largest_army);
    REQUIRE(g->longest_road() == gs.longest_road);
}

void check_player(k10engine::Game::Player* p, PlayerState ps)
{
    REQUIRE(p->can_accept_trade() == ps.can_accept_trade);
    REQUIRE(p->has_declined_trade() == ps.has_declined_trade);
    REQUIRE(p->num_to_discard() == ps.num_to_discard);
    REQUIRE(p->vertex() == ps.vertex);
    // REQUIRE(p->has_heavy_purse() == ps.has_heavy_purse);
    // REQUIRE(p->can_build() == ps.can_build);
    // REQUIRE(p->can_play() == ps.can_play);
    // REQUIRE(p->can_trade() == ps.can_trade);
    // REQUIRE(p->can_trade_with_bank() == ps.can_trade_with_bank);
    REQUIRE(p->is_current_player() == ps.is_current_player);
    // REQUIRE(p->is_blocking() == ps.is_blocking);
    REQUIRE(p->num_resources() == ps.num_resources);
    REQUIRE(p->army_size() == ps.army_size);
    REQUIRE(p->longest_road() == ps.longest_road);
    REQUIRE(p->public_victory_points() == ps.public_victory_points);
    REQUIRE(p->cities().size() == ps.cities);           // NOLINT(readability-container-size-empty)
    REQUIRE(p->roads().size() == ps.roads);             // NOLINT(readability-container-size-empty)
    REQUIRE(p->settlements().size() == ps.settlements); // NOLINT(readability-container-size-empty)
}

Action build(Building b, size_t node_id)
{
    return { Edge::Build,
             { { ArgType::BuildItemId, static_cast<size_t>(b) }, { ArgType::NodeId, node_id } } };
}

// NOLINTNEXTLINE(google-readability-function-size)
TEST_CASE("Single board", "[Game] [Game.Single]")
{
    SECTION("One player")
    {
        auto b = k10engine::Board::get_single_board();
        auto s = get_single_scenario();
        auto p = get_single_parameters();
        auto g = k10engine::Game::initialize(b, s, p);

        REQUIRE(g->hexes().size() == 1);
        REQUIRE(g->junctions().size() == 6);
        REQUIRE(g->roads().size() == 6);
        REQUIRE(g->players().size() == 1);

        GameState gs;
        auto ps = std::vector<PlayerState>(g->players().size());

        const auto check = [&]() -> void {
            check_game(g, gs);
            for (int i = 0; i < g->players().size(); ++i) {
                check_player(g->players().at(i), ps.at(i));
            }
        };

        const auto exec_result =
            [&](size_t player_id, const Action& action, ResType res_type) -> void {
            const auto& res = g->execute_action(player_id, action);
            REQUIRE(res.type == res_type);
        };

        const auto exec_ok = [&](size_t player_id, const Action& action) -> void {
            exec_result(player_id, action, ResType::Ok);
        };

        gs.robber_location = 9;
        ps[0].is_current_player = true;

        check();

        std::vector<Action> actions;

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

        exec_result(1, { Edge::AcceptTrade, {} }, ResType::InvalidPlayerId);
        exec_result(0, { Edge::AcceptTrade, {} }, ResType::InvalidEdgeChoice);
        exec_result(0, { Edge::Build, {} }, ResType::InvalidNumberOfArgs);
        exec_result(0,
                    { Edge::Build, { { ArgType::NodeId, 1 }, { ArgType::NodeId, 1 } } },
                    ResType::InvalidArgumentType);
        exec_result(0,
                    { Edge::Build,
                      { { ArgType::BuildItemId, static_cast<size_t>(Building::Settlement) },
                        { ArgType::BuildItemId, static_cast<size_t>(Building::Settlement) } } },
                    ResType::InvalidArgumentType);
        exec_result(0, build(Building::Settlement, 1000), ResType::NodeIdOutOfRange);
        exec_result(0, build(Building::Settlement, 1), ResType::InvalidNodeId);
        // NB: Can't test ::JunctionNotSettleable yet
        exec_ok(0, build(Building::Settlement, 2));

        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 1;
        ps[0].public_victory_points = 1;
        check();

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

        exec_result(1, { Edge::AcceptTrade, {} }, ResType::InvalidPlayerId);
        exec_result(0, { Edge::AcceptTrade, {} }, ResType::InvalidEdgeChoice);
        exec_result(0, { Edge::Build, {} }, ResType::InvalidNumberOfArgs);
        exec_result(0,
                    { Edge::Build, { { ArgType::NodeId, 4 }, { ArgType::NodeId, 4 } } },
                    ResType::InvalidArgumentType);
        exec_result(0,
                    { Edge::Build,
                      { { ArgType::BuildItemId, static_cast<size_t>(Building::Road) },
                        { ArgType::BuildItemId, static_cast<size_t>(Building::Road) } } },
                    ResType::InvalidArgumentType);
        exec_result(0, build(Building::Road, 1000), ResType::NodeIdOutOfRange);
        exec_result(0, build(Building::Road, 1), ResType::InvalidNodeId);
        exec_ok(0, build(Building::Road, 4));

        gs.is_first_round = false;
        gs.is_second_round = true;
        gs.round = 1;
        gs.turn = 1;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].roads = 1;
        check();

        actions = g->players().at(0)->get_available_actions();

        REQUIRE(actions.size() == 1);
        for (const auto& action : actions) {
            REQUIRE(action.edge == Edge::ToRoot);
            REQUIRE(action.args.size() == 0); // NOLINT(readability-container-size-empty)
        }

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check();

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

        exec_result(0, build(Building::Settlement, 5), ResType::JunctionNotSettleable);
        exec_result(0, build(Building::Settlement, 6), ResType::JunctionNotSettleable);
        exec_ok(0, build(Building::Settlement, 12));

        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 2;
        ps[0].public_victory_points = 2;
        check();

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

        exec_result(0, build(Building::Settlement, 13), ResType::InvalidEdgeChoice);
        exec_ok(0, build(Building::Road, 10));

        ps[0].vertex = Vertex::ChooseInitialResources;
        ps[0].roads = 2;
        check();

        actions = g->players().at(0)->get_available_actions();

        REQUIRE(actions.size() == 2);
        for (const auto& action : actions) {
            REQUIRE(action.edge == Edge::ChooseInitialResources);
            REQUIRE(action.args.size() == 1);
            REQUIRE(action.args.at(0).type == ArgType::NodeId);
            const auto node_id = action.args.at(0).value;
            REQUIRE(g->junctions().find(node_id) != g->junctions().end());
            const auto junction = g->junctions().at(node_id);
            REQUIRE(junction->owner() == g->players().at(0));
            const auto settlements = g->players().at(0)->settlements();
            REQUIRE(std::find(settlements.begin(), settlements.end(), junction)
                    != settlements.end());
        }

        exec_ok(0, { Edge::ChooseInitialResources, { { ArgType::NodeId, 12 } } });

        gs.is_second_round = false;
        gs.round = 2;
        gs.turn = 2;
        ps[0].vertex = Vertex::WaitForTurn;
        check();

        actions = g->players().at(0)->get_available_actions();

        REQUIRE(actions.size() == 1);
        for (const auto& action : actions) {
            REQUIRE(action.edge == Edge::ToRoot);
            REQUIRE(action.args.size() == 0); // NOLINT(readability-container-size-empty)
        }

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check();

        actions = g->players().at(0)->get_available_actions();

        REQUIRE(actions.size() == 1);
        for (const auto& action : actions) {
            REQUIRE(action.edge == Edge::RollDice);
            REQUIRE(action.args.size() == 0); // NOLINT(readability-container-size-empty)
        }

        for (const auto& action : g->players().at(0)->get_available_actions()) {
            std::cout << action << std::endl;
        }

        delete g;
        delete b;
    }
}

// NOLINTNEXTLINE(google-readability-function-size)
TEST_CASE("Triple board", "[Game] [Game.Triple]")
{
    SECTION("One player")
    {
        auto b = k10engine::Board::get_triple_board();
        auto s = get_triple_scenario();
        auto p = get_triple_parameters(1);
        auto g = k10engine::Game::initialize(b, s, p);

        REQUIRE(g->hexes().size() == 3);
        REQUIRE(g->junctions().size() == 13);
        REQUIRE(g->roads().size() == 15);
        REQUIRE(g->players().size() == 1);

        GameState gs;
        auto ps = std::vector<PlayerState>(g->players().size());

        const auto check = [&]() -> void {
            check_game(g, gs);
            for (int i = 0; i < g->players().size(); ++i) {
                check_player(g->players().at(i), ps.at(i));
            }
        };

        const auto exec_result =
            [&](size_t player_id, const Action& action, ResType res_type) -> void {
            const auto& res = g->execute_action(player_id, action);
            REQUIRE(res.type == res_type);
        };

        const auto exec_ok = [&](size_t player_id, const Action& action) -> void {
            exec_result(player_id, action, ResType::Ok);
        };

        gs.robber_location = 30;
        ps[0].is_current_player = true;

        check();

        std::vector<Action> actions;

        actions = g->players().at(0)->get_available_actions();
        REQUIRE(actions.size() == 13);
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

        exec_result(1, { Edge::AcceptTrade, {} }, ResType::InvalidPlayerId);
        exec_result(0, { Edge::AcceptTrade, {} }, ResType::InvalidEdgeChoice);
        exec_result(0, { Edge::Build, {} }, ResType::InvalidNumberOfArgs);
        exec_result(0,
                    { Edge::Build, { { ArgType::NodeId, 1 }, { ArgType::NodeId, 1 } } },
                    ResType::InvalidArgumentType);
        exec_result(0,
                    { Edge::Build,
                      { { ArgType::BuildItemId, static_cast<size_t>(Building::Settlement) },
                        { ArgType::BuildItemId, static_cast<size_t>(Building::Settlement) } } },
                    ResType::InvalidArgumentType);
        exec_result(0, build(Building::Settlement, 1000), ResType::NodeIdOutOfRange);
        exec_result(0, build(Building::Settlement, 1), ResType::InvalidNodeId);
        // NB: Can't test ::JunctionNotSettleable yet
        exec_ok(0, build(Building::Settlement, 20));

        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 1;
        ps[0].public_victory_points = 1;
        check();

        for (const auto& j_entry : g->junctions()) {
            const auto& j = j_entry.second;
            if (j->index() == 10 || j->index() == 20 || j->index() == 26 || j->index() == 27) {
                REQUIRE(j->is_settleable() == false);
            } else {
                REQUIRE(j->is_settleable() == true);
            }
            REQUIRE(j->owner() == (j->index() == 20 ? g->players().at(0) : nullptr));
        }

        actions = g->players().at(0)->get_available_actions();

        REQUIRE(actions.size() == 3);
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

        exec_result(1, { Edge::AcceptTrade, {} }, ResType::InvalidPlayerId);
        exec_result(0, { Edge::AcceptTrade, {} }, ResType::InvalidEdgeChoice);
        exec_result(0, { Edge::Build, {} }, ResType::InvalidNumberOfArgs);
        exec_result(0,
                    { Edge::Build, { { ArgType::NodeId, 4 }, { ArgType::NodeId, 4 } } },
                    ResType::InvalidArgumentType);
        exec_result(0,
                    { Edge::Build,
                      { { ArgType::BuildItemId, static_cast<size_t>(Building::Road) },
                        { ArgType::BuildItemId, static_cast<size_t>(Building::Road) } } },
                    ResType::InvalidArgumentType);
        exec_result(0, build(Building::Road, 1000), ResType::NodeIdOutOfRange);
        exec_result(0, build(Building::Road, 1), ResType::InvalidNodeId);
        exec_ok(0, build(Building::Road, 15));

        gs.is_first_round = false;
        gs.is_second_round = true;
        gs.round = 1;
        gs.turn = 1;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].roads = 1;
        check();

        actions = g->players().at(0)->get_available_actions();

        REQUIRE(actions.size() == 1);
        for (const auto& action : actions) {
            REQUIRE(action.edge == Edge::ToRoot);
            REQUIRE(action.args.size() == 0); // NOLINT(readability-container-size-empty)
        }

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check();

        actions = g->players().at(0)->get_available_actions();

        REQUIRE(actions.size() == 9);
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

        exec_result(0, build(Building::Settlement, 10), ResType::JunctionNotSettleable);
        exec_result(0, build(Building::Settlement, 26), ResType::JunctionNotSettleable);
        exec_result(0, build(Building::Settlement, 27), ResType::JunctionNotSettleable);
        exec_ok(0, build(Building::Settlement, 3));

        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 2;
        ps[0].public_victory_points = 2;
        check();

        actions = g->players().at(0)->get_available_actions();

        REQUIRE(actions.size() == 5);
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

        exec_result(0, build(Building::Settlement, 4), ResType::InvalidEdgeChoice);
        exec_ok(0, build(Building::Road, 7));

        ps[0].vertex = Vertex::ChooseInitialResources;
        ps[0].roads = 2;
        check();

        actions = g->players().at(0)->get_available_actions();

        REQUIRE(actions.size() == 2);
        for (const auto& action : actions) {
            REQUIRE(action.edge == Edge::ChooseInitialResources);
            REQUIRE(action.args.size() == 1);
            REQUIRE(action.args.at(0).type == ArgType::NodeId);
            const auto node_id = action.args.at(0).value;
            REQUIRE(g->junctions().find(node_id) != g->junctions().end());
            const auto junction = g->junctions().at(node_id);
            REQUIRE(junction->owner() == g->players().at(0));
            const auto settlements = g->players().at(0)->settlements();
            REQUIRE(std::find(settlements.begin(), settlements.end(), junction)
                    != settlements.end());
        }

        exec_ok(0, { Edge::ChooseInitialResources, { { ArgType::NodeId, 3 } } });

        gs.is_second_round = false;
        gs.round = 2;
        gs.turn = 2;
        ps[0].vertex = Vertex::WaitForTurn;
        check();

        actions = g->players().at(0)->get_available_actions();

        REQUIRE(actions.size() == 1);
        for (const auto& action : actions) {
            REQUIRE(action.edge == Edge::ToRoot);
            REQUIRE(action.args.size() == 0); // NOLINT(readability-container-size-empty)
        }

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check();

        actions = g->players().at(0)->get_available_actions();

        REQUIRE(actions.size() == 1);
        for (const auto& action : actions) {
            REQUIRE(action.edge == Edge::RollDice);
            REQUIRE(action.args.size() == 0); // NOLINT(readability-container-size-empty)
        }

        for (const auto& action : g->players().at(0)->get_available_actions()) {
            std::cout << action << std::endl;
        }

        delete g;
        delete b;
    }
}

} // namespace k10engine::Game
