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

void _exec_result(k10engine::Game::Game* g,
                  size_t player_id,
                  const Action& action,
                  const ResType& res_type)
{
    const auto& res = g->execute_action(player_id, action);
    REQUIRE(res.type == res_type);
}

void _check_no_actions(k10engine::Game::Game* g, size_t player_id)
{
    const auto actions = g->players().at(player_id)->get_available_actions();
    REQUIRE(actions.empty());
}

void _check_build_settlement(k10engine::Game::Game* g, size_t player_id, size_t num_expected)
{
    const auto invalid_player_id = 100;
    const auto large_node_id = 1000;
    const auto invalid_node_id = 0;
    const auto actions = g->players().at(player_id)->get_available_actions();
    REQUIRE(actions.size() == num_expected);
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
    _exec_result(
        g, invalid_player_id, build(Building::Settlement, large_node_id), ResType::InvalidPlayerId);
    _exec_result(g, player_id, { Edge::AcceptTrade, {} }, ResType::InvalidEdgeChoice);
    _exec_result(g, player_id, { Edge::Build, {} }, ResType::InvalidNumberOfArgs);
    _exec_result(g,
                 player_id,
                 { Edge::Build, { { ArgType::NodeId, 1 }, { ArgType::NodeId, 1 } } },
                 ResType::InvalidArgumentType);
    _exec_result(g,
                 player_id,
                 { Edge::Build,
                   { { ArgType::BuildItemId, static_cast<size_t>(Building::Settlement) },
                     { ArgType::BuildItemId, static_cast<size_t>(Building::Settlement) } } },
                 ResType::InvalidArgumentType);
    _exec_result(
        g, player_id, build(Building::Settlement, large_node_id), ResType::NodeIdOutOfRange);
    _exec_result(
        g, player_id, build(Building::Settlement, invalid_node_id), ResType::InvalidNodeId);
}

void _check_build_road(k10engine::Game::Game* g, size_t player_id, size_t num_expected)
{
    const auto invalid_player_id = 100;
    const auto large_node_id = 1000;
    const auto invalid_node_id = 0;
    const auto actions = g->players().at(player_id)->get_available_actions();
    REQUIRE(actions.size() == num_expected);
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
    _exec_result(
        g, invalid_player_id, build(Building::Road, large_node_id), ResType::InvalidPlayerId);
    _exec_result(g, player_id, { Edge::AcceptTrade, {} }, ResType::InvalidEdgeChoice);
    _exec_result(g, player_id, { Edge::Build, {} }, ResType::InvalidNumberOfArgs);
    _exec_result(g,
                 player_id,
                 { Edge::Build, { { ArgType::NodeId, 4 }, { ArgType::NodeId, 4 } } },
                 ResType::InvalidArgumentType);
    _exec_result(g,
                 player_id,
                 { Edge::Build,
                   { { ArgType::BuildItemId, static_cast<size_t>(Building::Road) },
                     { ArgType::BuildItemId, static_cast<size_t>(Building::Road) } } },
                 ResType::InvalidArgumentType);
    _exec_result(g, player_id, build(Building::Road, large_node_id), ResType::NodeIdOutOfRange);
    _exec_result(g, player_id, build(Building::Road, invalid_node_id), ResType::InvalidNodeId);
}

void _check_choose_initial_resources(k10engine::Game::Game* g, size_t player_id)
{
    const auto actions = g->players().at(player_id)->get_available_actions();
    REQUIRE(actions.size() == 2);
    for (const auto& action : actions) {
        REQUIRE(action.edge == Edge::ChooseInitialResources);
        REQUIRE(action.args.size() == 1);
        REQUIRE(action.args.at(0).type == ArgType::NodeId);
        const auto node_id = action.args.at(0).value;
        REQUIRE(g->junctions().find(node_id) != g->junctions().end());
        const auto junction = g->junctions().at(node_id);
        REQUIRE(junction->owner() == g->players().at(player_id));
        const auto settlements = g->players().at(player_id)->settlements();
        REQUIRE(std::find(settlements.begin(), settlements.end(), junction) != settlements.end());
    }
}

void _check_single_action(k10engine::Game::Game* g, size_t player_id, const Edge& edge)
{
    const auto actions = g->players().at(player_id)->get_available_actions();
    REQUIRE(actions.size() == 1);
    for (const auto& action : actions) {
        REQUIRE(action.edge == edge);
        REQUIRE(action.args.empty());
    }
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

        const auto check_state = [&]() -> void {
            check_game(g, gs);
            for (int i = 0; i < g->players().size(); ++i) {
                check_player(g->players().at(i), ps.at(i));
            }
        };
        const auto exec_result = [&](size_t pid, const Action& a, const ResType& rt) {
            _exec_result(g, pid, a, rt);
        };
        const auto exec_ok = [&](size_t pid, const Action& a) {
            _exec_result(g, pid, a, ResType::Ok);
        };
        const auto check_no_actions = [&](size_t pid) { _check_no_actions(g, pid); };
        const auto check_build_settlement = [&](size_t pid, size_t n) {
            _check_build_settlement(g, pid, n);
        };
        const auto check_build_road = [&](size_t pid, size_t n) { _check_build_road(g, pid, n); };
        const auto check_to_root = [&](size_t pid) { _check_single_action(g, pid, Edge::ToRoot); };
        const auto check_choose_initial_resources = [&](size_t pid) {
            _check_choose_initial_resources(g, pid);
        };
        const auto check_roll_dice = [&](size_t pid) {
            _check_single_action(g, pid, Edge::RollDice);
        };

        gs.robber_location = 9;
        ps[0].is_current_player = true;

        check_state();
        check_build_settlement(0, 6);

        exec_ok(0, build(Building::Settlement, 2));

        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 1;
        ps[0].public_victory_points = 1;
        check_state();
        check_build_road(0, 2);

        for (const auto& j_entry : g->junctions()) {
            const auto& j = j_entry.second;
            REQUIRE(j->is_settleable() == (j->index() > 6));
            REQUIRE(j->owner() == (j->index() == 2 ? g->players().at(0) : nullptr));
        }

        exec_ok(0, build(Building::Road, 4));

        gs.is_first_round = false;
        gs.is_second_round = true;
        gs.round = 1;
        gs.turn = 1;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].roads = 1;
        check_state();
        check_to_root(0);

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check_state();
        check_build_settlement(0, 3);

        exec_ok(0, build(Building::Settlement, 12));

        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 2;
        ps[0].public_victory_points = 2;
        check_state();
        check_build_road(0, 4);

        exec_ok(0, build(Building::Road, 10));

        ps[0].vertex = Vertex::ChooseInitialResources;
        ps[0].roads = 2;
        check_state();
        check_choose_initial_resources(0);

        exec_ok(0, { Edge::ChooseInitialResources, { { ArgType::NodeId, 12 } } });

        gs.is_second_round = false;
        gs.round = 2;
        gs.turn = 2;
        ps[0].vertex = Vertex::WaitForTurn;
        check_state();
        check_to_root(0);

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check_state();
        check_roll_dice(0);

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

        const auto check_state = [&]() -> void {
            check_game(g, gs);
            for (int i = 0; i < g->players().size(); ++i) {
                check_player(g->players().at(i), ps.at(i));
            }
        };
        const auto exec_result = [&](size_t pid, const Action& a, const ResType& rt) {
            _exec_result(g, pid, a, rt);
        };
        const auto exec_ok = [&](size_t pid, const Action& a) {
            _exec_result(g, pid, a, ResType::Ok);
        };
        const auto check_no_actions = [&](size_t pid) { _check_no_actions(g, pid); };
        const auto check_build_settlement = [&](size_t pid, size_t n) {
            _check_build_settlement(g, pid, n);
        };
        const auto check_build_road = [&](size_t pid, size_t n) { _check_build_road(g, pid, n); };
        const auto check_to_root = [&](size_t pid) { _check_single_action(g, pid, Edge::ToRoot); };
        const auto check_choose_initial_resources = [&](size_t pid) {
            _check_choose_initial_resources(g, pid);
        };
        const auto check_roll_dice = [&](size_t pid) {
            _check_single_action(g, pid, Edge::RollDice);
        };

        gs.robber_location = 30;
        ps[0].is_current_player = true;

        check_state();
        check_build_settlement(0, 13);

        exec_ok(0, build(Building::Settlement, 20));

        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 1;
        ps[0].public_victory_points = 1;
        check_state();
        check_build_road(0, 3);

        for (const auto& j_entry : g->junctions()) {
            const auto& j = j_entry.second;
            if (j->index() == 10 || j->index() == 20 || j->index() == 26 || j->index() == 27) {
                REQUIRE(j->is_settleable() == false);
            } else {
                REQUIRE(j->is_settleable() == true);
            }
            REQUIRE(j->owner() == (j->index() == 20 ? g->players().at(0) : nullptr));
        }

        exec_ok(0, build(Building::Road, 15));

        gs.is_first_round = false;
        gs.is_second_round = true;
        gs.round = 1;
        gs.turn = 1;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].roads = 1;
        check_state();
        check_to_root(0);

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check_state();
        check_build_settlement(0, 9);

        exec_ok(0, build(Building::Settlement, 3));

        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 2;
        ps[0].public_victory_points = 2;
        check_state();
        check_build_road(0, 5);

        exec_ok(0, build(Building::Road, 7));

        ps[0].vertex = Vertex::ChooseInitialResources;
        ps[0].roads = 2;
        check_state();
        check_choose_initial_resources(0);

        exec_ok(0, { Edge::ChooseInitialResources, { { ArgType::NodeId, 3 } } });

        gs.is_second_round = false;
        gs.round = 2;
        gs.turn = 2;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].num_resources = 1;
        check_state();
        check_to_root(0);

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check_state();
        check_roll_dice(0);

        delete g;
        delete b;
    }

    SECTION("Two players")
    {
        auto b = k10engine::Board::get_triple_board();
        auto s = get_triple_scenario();
        auto p = get_triple_parameters(2);
        auto g = k10engine::Game::initialize(b, s, p);

        REQUIRE(g->hexes().size() == 3);
        REQUIRE(g->junctions().size() == 13);
        REQUIRE(g->roads().size() == 15);
        REQUIRE(g->players().size() == 2);

        GameState gs;
        auto ps = std::vector<PlayerState>(g->players().size());

        const auto check_state = [&]() -> void {
            check_game(g, gs);
            for (int i = 0; i < g->players().size(); ++i) {
                check_player(g->players().at(i), ps.at(i));
            }
        };
        const auto exec_result = [&](size_t pid, const Action& a, const ResType& rt) {
            _exec_result(g, pid, a, rt);
        };
        const auto exec_ok = [&](size_t pid, const Action& a) {
            _exec_result(g, pid, a, ResType::Ok);
        };
        const auto check_no_actions = [&](size_t pid) { _check_no_actions(g, pid); };
        const auto check_build_settlement = [&](size_t pid, size_t n) {
            _check_build_settlement(g, pid, n);
        };
        const auto check_build_road = [&](size_t pid, size_t n) { _check_build_road(g, pid, n); };
        const auto check_to_root = [&](size_t pid) { _check_single_action(g, pid, Edge::ToRoot); };
        const auto check_choose_initial_resources = [&](size_t pid) {
            _check_choose_initial_resources(g, pid);
        };
        const auto check_roll_dice = [&](size_t pid) {
            _check_single_action(g, pid, Edge::RollDice);
        };

        gs.robber_location = 30;
        ps[0].is_current_player = true;
        ps[1].vertex = Vertex::WaitForTurn;

        check_state();
        check_build_settlement(0, 13);
        check_no_actions(1);

        exec_result(1, build(Building::Settlement, 20), ResType::InvalidEdgeChoice);
        exec_ok(0, build(Building::Settlement, 20));

        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 1;
        ps[0].public_victory_points = 1;
        check_state();
        check_build_road(0, 3);
        check_no_actions(1);

        for (const auto& j_entry : g->junctions()) {
            const auto& j = j_entry.second;
            if (j->index() == 10 || j->index() == 20 || j->index() == 26 || j->index() == 27) {
                REQUIRE(j->is_settleable() == false);
            } else {
                REQUIRE(j->is_settleable() == true);
            }
            REQUIRE(j->owner() == (j->index() == 20 ? g->players().at(0) : nullptr));
        }

        exec_result(1, build(Building::Settlement, 20), ResType::InvalidEdgeChoice);
        exec_ok(0, build(Building::Road, 15));

        gs.turn = 1;
        ps[0].is_current_player = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].roads = 1;
        ps[1].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_to_root(1);

        exec_result(0, { Edge::ToRoot, {} }, ResType::InvalidEdgeChoice);
        exec_ok(1, { Edge::ToRoot, {} });

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_build_settlement(1, 9);

        exec_ok(1, build(Building::Settlement, 3));

        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 1;
        ps[1].public_victory_points = 1;
        check_state();
        check_no_actions(0);
        check_build_road(1, 2);

        exec_result(0, build(Building::Settlement, 4), ResType::InvalidEdgeChoice);
        exec_ok(1, build(Building::Road, 6));

        gs.is_first_round = false;
        gs.is_second_round = true;
        gs.round = 1;
        gs.turn = 2;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].roads = 1;
        check_state();
        check_no_actions(0);
        check_to_root(1);

        exec_result(0, { Edge::ToRoot, {} }, ResType::InvalidEdgeChoice);
        exec_ok(1, { Edge::ToRoot, {} });

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_build_settlement(1, 7);

        exec_ok(1, build(Building::Settlement, 33));

        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 2;
        ps[1].public_victory_points = 2;
        check_state();
        check_no_actions(0);
        check_build_road(1, 4);

        exec_ok(1, build(Building::Road, 29));

        ps[1].vertex = Vertex::ChooseInitialResources;
        ps[1].roads = 2;
        check_state();
        check_no_actions(0);
        check_choose_initial_resources(1);

        exec_ok(1, { Edge::ChooseInitialResources, { { ArgType::NodeId, 3 } } });

        gs.turn = 3;
        ps[0].is_current_player = true;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].is_current_player = false;
        ps[1].num_resources = 1;
        check_state();
        check_to_root(0);
        check_no_actions(1);

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check_state();
        check_build_settlement(0, 5);
        check_no_actions(1);

        exec_ok(0, build(Building::Settlement, 34));

        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 2;
        ps[0].public_victory_points = 2;
        check_state();
        check_build_road(0, 5);
        check_no_actions(1);

        exec_ok(0, build(Building::Road, 7));

        ps[0].vertex = Vertex::ChooseInitialResources;
        ps[0].roads = 2;
        check_state();
        check_choose_initial_resources(0);
        check_no_actions(1);

        exec_ok(0, { Edge::ChooseInitialResources, { { ArgType::NodeId, 20 } } });

        gs.is_second_round = false;
        gs.turn = 4;
        gs.round = 2;
        ps[0].num_resources = 2;
        ps[0].vertex = Vertex::WaitForTurn;
        check_state();
        check_to_root(0);
        check_no_actions(1);

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check_state();
        check_roll_dice(0);
        check_no_actions(1);

        delete g;
        delete b;
    }
}

} // namespace k10engine::Game
