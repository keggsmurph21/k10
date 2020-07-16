#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>

#include "Board/Store.h"
#include "Game/Game.h"
#include "Scenario/Parameters.h"
#include "Scenario/Scenario.h"
#include "Scenario/Store.h"
#include "Test/catch.h"

using namespace k10engine; // NOLINT(google-build-using-namespace)

using Edge = State::Edge;
using Vertex = State::Vertex;
using Action = Game::Action;
using ActionArgument = Game::ActionArgument;
using ArgType = Game::ActionArgumentType;
using ResType = Game::ResultType;
using Building = Building;
using IterationType = Scenario::IterationType;
using Parameters = Scenario::Parameters;
using Scenario_ = Scenario::Scenario;

static Game::PlayerId s_next_player_id = 1;

inline Game::Game* make_game(Scenario::Name scenario_name,
                             size_t n_players,
                             size_t victory_points_goal,
                             Game::PlayerId next_player_id = 0)
{
    if (next_player_id == 0)
        next_player_id = s_next_player_id++;

    auto s = Scenario::Store::the().by_name(scenario_name);
    auto p = Scenario::Parameters{ Scenario::IterationType::Fixed,
                                   Scenario::IterationType::Fixed,
                                   Scenario::IterationType::Fixed,
                                   Scenario::IterationType::Fixed,
                                   n_players,
                                   victory_points_goal };
    auto* game = Game::Game::initialize(next_player_id++, s, p);
    return game;
}

struct GameState {
    bool can_steal = false;
    bool has_rolled = false;
    bool is_game_over = false;
    bool is_trade_accepted = false;
    bool is_first_round = true;
    bool is_second_round = false;
    bool is_roll_seven = false;
    bool should_wait_for_discard = false;
    bool has_current_trade = false;
    bool should_wait_for_trade = false;
    size_t num_trades_offered_this_turn = 0;
    size_t robber_location = 0;
    size_t dice_total = 0;
    size_t turn = 0;
    size_t round = 0;
    int has_largest_army = -1;
    size_t largest_army = 2;
    int has_longest_road = -1;
    size_t longest_road = 4;
    size_t cities_built = 0;
    size_t development_cards_built = 0;
    size_t roads_built = 0;
    size_t settlements_built = 0;
};

struct PlayerState {
    bool can_accept_trade = false;
    bool has_declined_trade = false;
    size_t num_to_discard = 0;
    Vertex vertex = Vertex::Root;
    bool is_current_player = false;
    size_t num_resources = 0;
    size_t num_played_development_cards = 0;
    size_t num_unplayed_development_cards = 0;
    size_t army_size = 0;
    size_t longest_road = 0;
    size_t public_victory_points = 0;
    size_t cities = 0;
    size_t roads = 0;
    size_t settlements = 0;
};

#define bootstrap_tests()                                                                                      \
                                                                                                               \
    GameState gs;                                                                                              \
    auto ps = std::vector<PlayerState>(g->players().size());                                                   \
    std::map<int, int> settlements;                                                                            \
    std::map<int, int> roads;                                                                                  \
                                                                                                               \
    const auto check_state = [&]() -> void {                                                                   \
        REQUIRE(g->can_steal() == gs.can_steal);                                                               \
        REQUIRE(g->has_rolled() == gs.has_rolled);                                                             \
        REQUIRE(g->is_game_over() == gs.is_game_over);                                                         \
        REQUIRE(g->is_trade_accepted() == gs.is_trade_accepted);                                               \
        REQUIRE(g->is_first_round() == gs.is_first_round);                                                     \
        REQUIRE(g->is_second_round() == gs.is_second_round);                                                   \
        REQUIRE(g->is_roll_seven() == gs.is_roll_seven);                                                       \
        REQUIRE(g->should_wait_for_discard() == gs.should_wait_for_discard);                                   \
        REQUIRE(g->has_current_trade() == gs.has_current_trade);                                               \
        REQUIRE(g->should_wait_for_trade() == gs.should_wait_for_trade);                                       \
        REQUIRE(g->num_trades_offered_this_turn() == gs.num_trades_offered_this_turn);                         \
        REQUIRE(g->robber_location()->node().index() == gs.robber_location);                                   \
        REQUIRE(g->get_dice_total() == gs.dice_total);                                                         \
        REQUIRE(g->turn() == gs.turn);                                                                         \
        REQUIRE(g->get_round() == gs.round);                                                                   \
        if (gs.has_largest_army == -1) {                                                                       \
            REQUIRE(g->has_largest_army() == nullptr);                                                         \
        } else {                                                                                               \
            REQUIRE(g->has_largest_army()->index() == gs.has_largest_army);                                    \
        }                                                                                                      \
        REQUIRE(g->largest_army() == gs.largest_army);                                                         \
        if (gs.has_longest_road == -1) {                                                                       \
            REQUIRE(g->has_longest_road() == nullptr);                                                         \
        } else {                                                                                               \
            REQUIRE(g->has_longest_road()->index() == gs.has_longest_road);                                    \
        }                                                                                                      \
        REQUIRE(g->longest_road() == gs.longest_road);                                                         \
        REQUIRE(g->num_built(Building::City) == gs.cities_built);                                              \
        REQUIRE(g->num_built(Building::DevelopmentCard) == gs.development_cards_built);                        \
        REQUIRE(g->num_built(Building::Road) == gs.roads_built);                                               \
        REQUIRE(g->num_built(Building::Settlement) == gs.settlements_built);                                   \
        for (int i = 0; i < g->players().size(); ++i) {                                                        \
            REQUIRE(g->player(i).can_accept_trade() == ps.at(i).can_accept_trade);                             \
            REQUIRE(g->player(i).has_declined_trade() == ps.at(i).has_declined_trade);                         \
            REQUIRE(g->player(i).num_to_discard() == ps.at(i).num_to_discard);                                 \
            REQUIRE(g->player(i).vertex() == ps.at(i).vertex);                                                 \
            REQUIRE(g->player(i).is_current_player() == ps.at(i).is_current_player);                           \
            REQUIRE(g->player(i).num_resources() == ps.at(i).num_resources);                                   \
            REQUIRE(g->player(i).num_played_development_cards() == ps.at(i).num_played_development_cards);     \
            REQUIRE(g->player(i).num_unplayed_development_cards() == ps.at(i).num_unplayed_development_cards); \
            REQUIRE(g->player(i).army_size() == ps.at(i).army_size);                                           \
            REQUIRE(g->player(i).longest_road() == ps.at(i).longest_road);                                     \
            REQUIRE(g->player(i).public_victory_points() == ps.at(i).public_victory_points);                   \
            REQUIRE(g->player(i).cities().size() == ps.at(i).cities);                                          \
            REQUIRE(g->player(i).roads().size() == ps.at(i).roads);                                            \
            REQUIRE(g->player(i).settlements().size() == ps.at(i).settlements);                                \
        }                                                                                                      \
    };                                                                                                         \
                                                                                                               \
    const auto check_no_actions = [&](size_t player_index) {                                                   \
        const auto actions = g->player(player_index).get_available_actions();                                  \
        REQUIRE(actions.empty());                                                                              \
    };                                                                                                         \
                                                                                                               \
    const auto check_build_settlement = [&](size_t player_index, size_t num_expected) {                        \
        const auto actions = g->player(player_index).get_available_actions();                                  \
        size_t num_buildable_settlements_found = 0;                                                            \
        for (const auto& action : actions) {                                                                   \
            if (action.edge != Edge::BuildSettlement) {                                                        \
                continue;                                                                                      \
            }                                                                                                  \
            if (static_cast<Building>(action.args.at(0).value) != Building::Settlement) {                      \
                continue;                                                                                      \
            }                                                                                                  \
            ++num_buildable_settlements_found;                                                                 \
            REQUIRE(action.args.size() == 2);                                                                  \
            REQUIRE(action.args.at(0).type == ArgType::BuildItemId);                                           \
            REQUIRE(action.args.at(0).value == static_cast<size_t>(Building::Settlement));                     \
            REQUIRE(action.args.at(1).type == ArgType::NodeId);                                                \
            const auto node_index = action.args.at(1).value;                                                   \
            REQUIRE(g->junction(node_index) != nullptr);                                                       \
            REQUIRE(g->junction(node_index)->is_settleable() == true);                                         \
        }                                                                                                      \
        REQUIRE(num_buildable_settlements_found == num_expected);                                              \
        REQUIRE(g->execute_accept_trade(g->player(player_index)).type == ResType::InvalidEdgeChoice);          \
    };                                                                                                         \
                                                                                                               \
    const auto check_build_road = [&](size_t player_index, size_t num_expected) {                              \
        const auto actions = g->player(player_index).get_available_actions();                                  \
        size_t num_buildable_roads_found = 0;                                                                  \
        for (const auto& action : actions) {                                                                   \
            if (action.edge != Edge::BuildRoad) {                                                              \
                continue;                                                                                      \
            }                                                                                                  \
            if (static_cast<Building>(action.args.at(0).value) != Building::Road) {                            \
                continue;                                                                                      \
            }                                                                                                  \
            ++num_buildable_roads_found;                                                                       \
            REQUIRE(action.edge == Edge::BuildRoad);                                                           \
            REQUIRE(action.args.size() == 2);                                                                  \
            REQUIRE(action.args.at(0).type == ArgType::BuildItemId);                                           \
            REQUIRE(action.args.at(0).value == static_cast<size_t>(Building::Road));                           \
            REQUIRE(action.args.at(1).type == ArgType::NodeId);                                                \
            const auto node_index = action.args.at(1).value;                                                   \
            REQUIRE(g->road(node_index) != nullptr);                                                           \
            REQUIRE(g->road(node_index)->owner() == nullptr);                                                  \
        }                                                                                                      \
        REQUIRE(num_buildable_roads_found == num_expected);                                                    \
        REQUIRE(g->execute_accept_trade(g->player(player_index)).type == ResType::InvalidEdgeChoice);          \
    };                                                                                                         \
                                                                                                               \
    const auto check_choose_initial_resources = [&](size_t player_index) {                                     \
        const auto actions = g->player(player_index).get_available_actions();                                  \
        REQUIRE(actions.size() == 2);                                                                          \
        for (const auto& action : actions) {                                                                   \
            REQUIRE(action.edge == Edge::ChooseInitialResources);                                              \
            REQUIRE(action.args.size() == 1);                                                                  \
            REQUIRE(action.args.at(0).type == ArgType::NodeId);                                                \
            const auto node_index = action.args.at(0).value;                                                   \
            REQUIRE(g->junction(node_index) != nullptr);                                                       \
            const auto junction = g->junction(node_index);                                                     \
            REQUIRE(junction->owner() != nullptr);                                                             \
            REQUIRE(*junction->owner() == g->player(player_index));                                            \
            const auto settlements = g->player(player_index).settlements();                                    \
            REQUIRE(std::find(settlements.begin(), settlements.end(), junction) != settlements.end());         \
        }                                                                                                      \
    };                                                                                                         \
                                                                                                               \
    const auto check_single_action = [&](size_t player_index, const Edge& edge) {                              \
        const auto actions = g->player(player_index).get_available_actions();                                  \
        REQUIRE(actions.size() == 1);                                                                          \
        for (const auto& action : actions) {                                                                   \
            REQUIRE(action.edge == edge);                                                                      \
            REQUIRE(action.args.empty());                                                                      \
        }                                                                                                      \
    };                                                                                                         \
                                                                                                               \
    const auto check_to_root = [&](size_t player_index) { check_single_action(player_index, Edge::ToRoot); };  \
                                                                                                               \
    const auto check_end_turn = [&](size_t player_index) {                                                     \
        const auto actions = g->player(player_index).get_available_actions();                                  \
        bool found = false;                                                                                    \
        for (const auto& action : actions) {                                                                   \
            if (action.edge == Edge::EndTurn) {                                                                \
                found = true;                                                                                  \
                break;                                                                                         \
            }                                                                                                  \
        }                                                                                                      \
        REQUIRE(found == true);                                                                                \
    };                                                                                                         \
                                                                                                               \
    const auto check_roll_dice = [&](size_t player_index) {                                                    \
        check_single_action(player_index, Edge::RollDice);                                                     \
        REQUIRE(g->execute_roll_dice(g->player(player_index), 0).type == ResType::DiceRollOutOfRange);         \
        REQUIRE(g->execute_roll_dice(g->player(player_index), 1).type == ResType::DiceRollOutOfRange);         \
        REQUIRE(g->execute_roll_dice(g->player(player_index), 13).type == ResType::DiceRollOutOfRange);        \
    };                                                                                                         \
                                                                                                               \
    const auto check_settlements = [&]() {                                                                     \
        g->for_each_junction([&](const auto* j) {                                                              \
            const auto& map_it = settlements.find(j->index());                                                 \
            if (map_it == settlements.end()) {                                                                 \
                REQUIRE(j->is_settleable() == true);                                                           \
                REQUIRE(j->owner() == nullptr);                                                                \
            } else {                                                                                           \
                REQUIRE(j->is_settleable() == false);                                                          \
                const auto& player_index = settlements.at(j->index());                                         \
                if (player_index == -1) {                                                                      \
                    REQUIRE(j->owner() == nullptr);                                                            \
                } else {                                                                                       \
                    REQUIRE(j->owner() != nullptr);                                                            \
                    REQUIRE(j->owner()->index() == player_index);                                              \
                }                                                                                              \
            }                                                                                                  \
        });                                                                                                    \
    };                                                                                                         \
                                                                                                               \
    const auto check_roads = [&]() {                                                                           \
        g->for_each_road([&](const auto* r) {                                                                  \
            const auto& map_it = roads.find(r->index());                                                       \
            if (map_it == roads.end()) {                                                                       \
                REQUIRE(r->owner() == nullptr);                                                                \
            } else {                                                                                           \
                const auto& player_index = roads.at(r->index());                                               \
                REQUIRE(r->owner()->index() == player_index);                                                  \
            }                                                                                                  \
        });                                                                                                    \
    };                                                                                                         \
                                                                                                               \
    const auto dump_actions = [&]() {                                                                          \
        std::cout << *g << std::endl;                                                                          \
        for (const auto& player : g->players()) {                                                              \
            std::cout << player << std::endl;                                                                  \
            for (const auto& action : player->get_available_actions()) {                                       \
                std::cout << " - " << action << std::endl;                                                     \
            }                                                                                                  \
        }                                                                                                      \
        std::cout << std::endl;                                                                                \
    };                                                                                                         \
                                                                                                               \
    const auto do_first_two_rounds_standard_3p = [&]() {                                                       \
        gs.robber_location = 143;                                                                              \
        REQUIRE(g->execute_build_settlement(g->player(0), g->junction(4)).type == ResType::Ok);                \
        REQUIRE(g->execute_build_road(g->player(0), g->road(7)).type == ResType::Ok);                          \
        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);                                         \
        REQUIRE(g->execute_build_settlement(g->player(1), g->junction(5)).type == ResType::Ok);                \
        REQUIRE(g->execute_build_road(g->player(1), g->road(9)).type == ResType::Ok);                          \
        REQUIRE(g->execute_to_root(g->player(2)).type == ResType::Ok);                                         \
        REQUIRE(g->execute_build_settlement(g->player(2), g->junction(90)).type == ResType::Ok);               \
        REQUIRE(g->execute_build_road(g->player(2), g->road(80)).type == ResType::Ok);                         \
        REQUIRE(g->execute_to_root(g->player(2)).type == ResType::Ok);                                         \
        REQUIRE(g->execute_build_settlement(g->player(2), g->junction(91)).type == ResType::Ok);               \
        REQUIRE(g->execute_build_road(g->player(2), g->road(63)).type == ResType::Ok);                         \
        REQUIRE(g->execute_choose_initial_resources(g->player(2), g->junction(91)).type == ResType::Ok);       \
        REQUIRE(g->execute_to_root(g->player(1)).type == ResType::Ok);                                         \
        REQUIRE(g->execute_build_settlement(g->player(1), g->junction(6)).type == ResType::Ok);                \
        REQUIRE(g->execute_build_road(g->player(1), g->road(10)).type == ResType::Ok);                         \
        REQUIRE(g->execute_choose_initial_resources(g->player(1), g->junction(6)).type == ResType::Ok);        \
        REQUIRE(g->execute_to_root(g->player(0)).type == ResType::Ok);                                         \
        REQUIRE(g->execute_build_settlement(g->player(0), g->junction(26)).type == ResType::Ok);               \
        REQUIRE(g->execute_build_road(g->player(0), g->road(18)).type == ResType::Ok);                         \
        REQUIRE(g->execute_choose_initial_resources(g->player(0), g->junction(26)).type == ResType::Ok);       \
                                                                                                               \
        gs.is_first_round = false;                                                                             \
        gs.is_second_round = false;                                                                            \
        gs.roads_built = 6;                                                                                    \
        gs.settlements_built = 6;                                                                              \
        gs.turn = 6;                                                                                           \
        gs.round = 2;                                                                                          \
        ps[0].vertex = Vertex::WaitForTurn;                                                                    \
        ps[0].num_resources = 2;                                                                               \
        ps[0].is_current_player = true;                                                                        \
        ps[0].settlements = 2;                                                                                 \
        ps[0].roads = 2;                                                                                       \
        ps[0].public_victory_points = 2;                                                                       \
        ps[1].vertex = Vertex::WaitForTurn;                                                                    \
        ps[1].num_resources = 1;                                                                               \
        ps[1].settlements = 2;                                                                                 \
        ps[1].roads = 2;                                                                                       \
        ps[1].public_victory_points = 2;                                                                       \
        ps[2].vertex = Vertex::WaitForTurn;                                                                    \
        ps[2].num_resources = 3;                                                                               \
        ps[2].settlements = 2;                                                                                 \
        ps[2].roads = 2;                                                                                       \
        ps[2].public_victory_points = 2;                                                                       \
        roads[7] = 0;                                                                                          \
        roads[9] = 1;                                                                                          \
        roads[10] = 1;                                                                                         \
        roads[18] = 0;                                                                                         \
        roads[63] = 2;                                                                                         \
        roads[80] = 2;                                                                                         \
        settlements[4] = 0;                                                                                    \
        settlements[5] = 1;                                                                                    \
        settlements[6] = 1;                                                                                    \
        settlements[13] = -1;                                                                                  \
        settlements[14] = -1;                                                                                  \
        settlements[15] = -1;                                                                                  \
        settlements[16] = -1;                                                                                  \
        settlements[26] = 0;                                                                                   \
        settlements[38] = -1;                                                                                  \
        settlements[39] = -1;                                                                                  \
        settlements[71] = -1;                                                                                  \
        settlements[72] = -1;                                                                                  \
        settlements[90] = 2;                                                                                   \
        settlements[91] = 2;                                                                                   \
        settlements[105] = -1;                                                                                 \
        settlements[106] = -1;                                                                                 \
        settlements[107] = -1;                                                                                 \
        check_state();                                                                                         \
        check_to_root(0);                                                                                      \
        check_no_actions(1);                                                                                   \
        check_no_actions(2);                                                                                   \
    };
