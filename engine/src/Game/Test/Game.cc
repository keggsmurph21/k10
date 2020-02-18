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
using Action = k10engine::Game::Action;
using ArgType = k10engine::Game::ActionArgumentType;
using ResType = k10engine::Game::ResultType;
using Building = k10engine::Building;

Action build(Building b, size_t node_id)
{
    return { Edge::Build,
             { { ArgType::BuildItemId, static_cast<size_t>(b) }, { ArgType::NodeId, node_id } } };
}

// NOLINTNEXTLINE(google-readability-function-size)
TEST_CASE("Game initialization", "[Game]")
{
    SECTION("Single")
    {
        auto b = k10engine::Board::get_single_board();
        auto s = get_single_scenario();
        auto p = get_single_parameters();
        auto g = k10engine::Game::initialize(b, s, p);

        bool can_steal = false;
        bool has_rolled = false;
        bool is_game_over = false;
        bool is_trade_accepted = false;
        bool is_first_round = true;
        bool is_second_round = false;
        bool is_roll_seven = false;
        // bool should_wait_for_discard = false; // FIXME: Implement
        // bool should_wait_for_trade = false; // FIXME: Implement
        size_t robber_location = 9;
        size_t dice_total = 0;
        size_t turn = 0;
        size_t round = 0;
        size_t largest_army = 2;
        size_t longest_road = 4;
        bool p0_can_accept_trade = false;
        bool p0_has_declined_trade = false;
        size_t p0_num_to_discard = 0;
        Vertex p0_vertex = Vertex::Root;
        // bool p0_has_heavy_purse = false; // FIXME: Implement
        // bool p0_can_build = false; // FIXME: Implement
        // bool p0_can_play = false; // FIXME: Implement
        // bool p0_can_trade = false; // FIXME: Implement
        // bool p0_can_trade_with_bank = false; // FIXME: Implement
        bool p0_is_current_player = true;
        // bool p0_is_blocking = false; // FIXME: Implement
        size_t p0_num_resources = 0;
        size_t p0_army_size = 0;
        size_t p0_longest_road = 0;
        size_t p0_public_victory_points = 0;
        size_t p0_cities = 0;
        size_t p0_roads = 0;
        size_t p0_settlements = 0;

        const auto check = [&]() -> void {
            REQUIRE(g->can_steal() == can_steal);
            REQUIRE(g->has_rolled() == has_rolled);
            REQUIRE(g->is_game_over() == is_game_over);
            REQUIRE(g->is_trade_accepted() == is_trade_accepted);
            REQUIRE(g->is_first_round() == is_first_round);
            REQUIRE(g->is_second_round() == is_second_round);
            REQUIRE(g->is_roll_seven() == is_roll_seven);
            // REQUIRE(g->should_wait_for_discard() == should_wait_for_discard);
            // REQUIRE(g->should_wait_for_trade() == should_wait_for_trade);
            REQUIRE(g->robber_location()->node()->index() == robber_location);
            REQUIRE(g->get_dice_total() == dice_total);
            REQUIRE(g->turn() == turn);
            REQUIRE(g->get_round() == round);
            REQUIRE(g->largest_army() == largest_army);
            REQUIRE(g->longest_road() == longest_road);

            k10engine::Game::Player* p;

            p = g->players().at(0);
            REQUIRE(p->can_accept_trade() == p0_can_accept_trade);
            REQUIRE(p->has_declined_trade() == p0_has_declined_trade);
            REQUIRE(p->num_to_discard() == p0_num_to_discard);
            REQUIRE(p->vertex() == p0_vertex);
            // REQUIRE(p->has_heavy_purse() == p0_has_heavy_purse);
            // REQUIRE(p->can_build() == p0_can_build);
            // REQUIRE(p->can_play() == p0_can_play);
            // REQUIRE(p->can_trade() == p0_can_trade);
            // REQUIRE(p->can_trade_with_bank() == p0_can_trade_with_bank);
            REQUIRE(p->is_current_player() == p0_is_current_player);
            // REQUIRE(p->is_blocking() == p0_is_blocking);
            REQUIRE(p->num_resources() == p0_num_resources);
            REQUIRE(p->army_size() == p0_army_size);
            REQUIRE(p->longest_road() == p0_longest_road);
            REQUIRE(p->public_victory_points() == p0_public_victory_points);
            REQUIRE(p->cities().size() == p0_cities); // NOLINT(readability-container-size-empty)
            REQUIRE(p->roads().size() == p0_roads);   // NOLINT(readability-container-size-empty)
            REQUIRE(p->settlements().size()
                    == p0_settlements); // NOLINT(readability-container-size-empty)
        };

        const auto exec_result =
            [&](size_t player_id, const Action& action, ResType res_type) -> void {
            const auto& res = g->execute_action(player_id, action);
            REQUIRE(res.type == res_type);
        };

        const auto exec_ok = [&](size_t player_id, const Action& action) -> void {
            exec_result(player_id, action, ResType::Ok);
        };

        check();

        REQUIRE(g->hexes().size() == 1);
        REQUIRE(g->junctions().size() == 6);
        REQUIRE(g->roads().size() == 6);
        REQUIRE(g->players().size() == 1);

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

        p0_vertex = Vertex::AfterBuildingFreeSettlement;
        p0_settlements = 1;
        p0_public_victory_points = 1;
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

        is_first_round = false;
        is_second_round = true;
        round = 1;
        turn = 1;
        p0_vertex = Vertex::WaitForTurn;
        p0_roads = 1;
        check();

        actions = g->players().at(0)->get_available_actions();

        REQUIRE(actions.size() == 1);
        for (const auto& action : actions) {
            REQUIRE(action.edge == Edge::ToRoot);
            REQUIRE(action.args.size() == 0); // NOLINT(readability-container-size-empty)
        }

        exec_ok(0, { Edge::ToRoot, {} });

        p0_vertex = Vertex::Root;
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

        p0_vertex = Vertex::AfterBuildingFreeSettlement;
        p0_settlements = 2;
        p0_public_victory_points = 2;
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

        p0_vertex = Vertex::ChooseInitialResources;
        p0_roads = 2;
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

        is_second_round = false;
        round = 2;
        turn = 2;
        p0_vertex = Vertex::WaitForTurn;
        check();

        actions = g->players().at(0)->get_available_actions();

        REQUIRE(actions.size() == 1);
        for (const auto& action : actions) {
            REQUIRE(action.edge == Edge::ToRoot);
            REQUIRE(action.args.size() == 0); // NOLINT(readability-container-size-empty)
        }

        exec_ok(0, { Edge::ToRoot, {} });

        p0_vertex = Vertex::Root;
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
