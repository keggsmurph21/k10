#include <iostream>
#include <stdexcept>
#include <vector>

#include "Board/Parser.h"
#include "Game/Game.h"
#include "Scenario/Parameters.h"
#include "Scenario/Scenario.h"
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

struct GameState {
    bool can_steal = false;
    bool has_rolled = false;
    bool is_game_over = false;
    bool is_trade_accepted = false;
    bool is_first_round = true;
    bool is_second_round = false;
    bool is_roll_seven = false;
    // bool should_wait_for_discard = false; // FIXME: Implement:
    // REQUIRE(g->should_wait_for_discard() == gs.should_wait_for_discard);
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
    // bool has_heavy_purse = false; // FIXME: Implement
    // REQUIRE(p->has_heavy_purse() == ps.has_heavy_purse);
    // bool can_build = false; // FIXME: Implement
    // REQUIRE(p->can_build() == ps.can_build);
    // bool can_play = false; // FIXME: Implement
    // REQUIRE(p->can_play() == ps.can_play);
    // bool can_trade = false; // FIXME: Implement
    // REQUIRE(p->can_trade() == ps.can_trade);
    // bool can_trade_with_bank = false; // FIXME: Implement
    // REQUIRE(p->can_trade_with_bank() == ps.can_trade_with_bank);
    bool is_current_player = false;
    // bool is_blocking = false; // FIXME: Implement
    // REQUIRE(p->is_blocking() == ps.is_blocking);
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

#define bootstrap_tests()                                                                          \
                                                                                                   \
    GameState gs;                                                                                  \
    auto ps = std::vector<PlayerState>(g->players().size());                                       \
    std::map<int, int> settlements;                                                                \
    std::map<int, int> roads;                                                                      \
                                                                                                   \
    const auto check_state = [&]() -> void {                                                       \
        REQUIRE(g->can_steal() == gs.can_steal);                                                   \
        REQUIRE(g->has_rolled() == gs.has_rolled);                                                 \
        REQUIRE(g->is_game_over() == gs.is_game_over);                                             \
        REQUIRE(g->is_trade_accepted() == gs.is_trade_accepted);                                   \
        REQUIRE(g->is_first_round() == gs.is_first_round);                                         \
        REQUIRE(g->is_second_round() == gs.is_second_round);                                       \
        REQUIRE(g->is_roll_seven() == gs.is_roll_seven);                                           \
        REQUIRE(g->has_current_trade() == gs.has_current_trade);                                   \
        REQUIRE(g->should_wait_for_trade() == gs.should_wait_for_trade);                           \
        REQUIRE(g->num_trades_offered_this_turn() == gs.num_trades_offered_this_turn);             \
        REQUIRE(g->robber_location()->node()->index() == gs.robber_location);                      \
        REQUIRE(g->get_dice_total() == gs.dice_total);                                             \
        REQUIRE(g->turn() == gs.turn);                                                             \
        REQUIRE(g->get_round() == gs.round);                                                       \
        if (gs.has_largest_army == -1) {                                                           \
            REQUIRE(g->has_largest_army() == nullptr);                                             \
        } else {                                                                                   \
            REQUIRE(g->has_largest_army()->index() == gs.has_largest_army);                        \
        }                                                                                          \
        REQUIRE(g->largest_army() == gs.largest_army);                                             \
        if (gs.has_longest_road == -1) {                                                           \
            REQUIRE(g->has_longest_road() == nullptr);                                             \
        } else {                                                                                   \
            REQUIRE(g->has_longest_road()->index() == gs.has_longest_road);                        \
        }                                                                                          \
        REQUIRE(g->longest_road() == gs.longest_road);                                             \
        REQUIRE(g->num_built(Building::City) == gs.cities_built);                                  \
        REQUIRE(g->num_built(Building::DevelopmentCard) == gs.development_cards_built);            \
        REQUIRE(g->num_built(Building::Road) == gs.roads_built);                                   \
        REQUIRE(g->num_built(Building::Settlement) == gs.settlements_built);                       \
        for (int i = 0; i < g->players().size(); ++i) {                                            \
            REQUIRE(g->players().at(i)->can_accept_trade() == ps.at(i).can_accept_trade);          \
            REQUIRE(g->players().at(i)->has_declined_trade() == ps.at(i).has_declined_trade);      \
            REQUIRE(g->players().at(i)->num_to_discard() == ps.at(i).num_to_discard);              \
            REQUIRE(g->players().at(i)->vertex() == ps.at(i).vertex);                              \
            REQUIRE(g->players().at(i)->is_current_player() == ps.at(i).is_current_player);        \
            REQUIRE(g->players().at(i)->num_resources() == ps.at(i).num_resources);                \
            REQUIRE(g->players().at(i)->num_played_development_cards()                             \
                    == ps.at(i).num_played_development_cards);                                     \
            REQUIRE(g->players().at(i)->num_unplayed_development_cards()                           \
                    == ps.at(i).num_unplayed_development_cards);                                   \
            REQUIRE(g->players().at(i)->army_size() == ps.at(i).army_size);                        \
            REQUIRE(g->players().at(i)->longest_road() == ps.at(i).longest_road);                  \
            REQUIRE(g->players().at(i)->public_victory_points()                                    \
                    == ps.at(i).public_victory_points);                                            \
            REQUIRE(g->players().at(i)->cities().size() == ps.at(i).cities);                       \
            REQUIRE(g->players().at(i)->roads().size() == ps.at(i).roads);                         \
            REQUIRE(g->players().at(i)->settlements().size() == ps.at(i).settlements);             \
        }                                                                                          \
    };                                                                                             \
                                                                                                   \
    const auto exec_error =                                                                        \
        [&](size_t player_index, const Action& action, const ResType& res_type) {                  \
            const auto& res = g->execute_action(player_index, action);                             \
            REQUIRE(res.type == res_type);                                                         \
        };                                                                                         \
                                                                                                   \
    const auto exec_ok = [&](size_t player_index, const Action& action) {                          \
        exec_error(player_index, action, ResType::Ok);                                             \
    };                                                                                             \
                                                                                                   \
    const auto check_no_actions = [&](size_t player_index) {                                       \
        const auto actions = g->players().at(player_index)->get_available_actions();               \
        REQUIRE(actions.empty());                                                                  \
    };                                                                                             \
                                                                                                   \
    const auto check_build_settlement = [&](size_t player_index, size_t num_expected) {            \
        const auto invalid_player_index = 100;                                                     \
        const auto large_node_index = 1000;                                                        \
        const auto invalid_node_index = 0;                                                         \
        const auto actions = g->players().at(player_index)->get_available_actions();               \
        size_t num_buildable_settlements_found = 0;                                                \
        for (const auto& action : actions) {                                                       \
            if (action.edge != Edge::Build) {                                                      \
                continue;                                                                          \
            }                                                                                      \
            if (static_cast<Building>(action.args.at(0).value) != Building::Settlement) {          \
                continue;                                                                          \
            }                                                                                      \
            ++num_buildable_settlements_found;                                                     \
            REQUIRE(action.args.size() == 2);                                                      \
            REQUIRE(action.args.at(0).type == ArgType::BuildItemId);                               \
            REQUIRE(action.args.at(0).value == static_cast<size_t>(Building::Settlement));         \
            REQUIRE(action.args.at(1).type == ArgType::NodeId);                                    \
            const auto node_index = action.args.at(1).value;                                       \
            REQUIRE(g->junctions().find(node_index) != g->junctions().end());                      \
            REQUIRE(g->junctions().at(node_index)->is_settleable() == true);                       \
        }                                                                                          \
        REQUIRE(num_buildable_settlements_found == num_expected);                                  \
        exec_error(invalid_player_index,                                                           \
                   build(Building::Settlement, large_node_index),                                  \
                   ResType::InvalidPlayerId);                                                      \
        exec_error(player_index, { Edge::AcceptTrade, {} }, ResType::InvalidEdgeChoice);           \
        exec_error(player_index, { Edge::Build, {} }, ResType::InvalidNumberOfArgs);               \
        exec_error(player_index,                                                                   \
                   { Edge::Build, { { ArgType::NodeId, 1 }, { ArgType::NodeId, 1 } } },            \
                   ResType::InvalidArgumentType);                                                  \
        exec_error(player_index,                                                                   \
                   { Edge::Build,                                                                  \
                     { { ArgType::BuildItemId, static_cast<size_t>(Building::Settlement) },        \
                       { ArgType::BuildItemId, static_cast<size_t>(Building::Settlement) } } },    \
                   ResType::InvalidNodeId);                                                        \
        exec_error(                                                                                \
            player_index, build(Building::Settlement, large_node_index), ResType::InvalidNodeId);  \
        exec_error(player_index,                                                                   \
                   build(Building::Settlement, invalid_node_index),                                \
                   ResType::InvalidNodeId);                                                        \
    };                                                                                             \
                                                                                                   \
    const auto check_build_road = [&](size_t player_index, size_t num_expected) {                  \
        const auto invalid_player_index = 100;                                                     \
        const auto large_node_index = 1000;                                                        \
        const auto invalid_node_index = 0;                                                         \
        const auto actions = g->players().at(player_index)->get_available_actions();               \
        size_t num_buildable_roads_found = 0;                                                      \
        for (const auto& action : actions) {                                                       \
            if (action.edge != Edge::Build) {                                                      \
                continue;                                                                          \
            }                                                                                      \
            if (static_cast<Building>(action.args.at(0).value) != Building::Road) {                \
                continue;                                                                          \
            }                                                                                      \
            ++num_buildable_roads_found;                                                           \
            REQUIRE(action.edge == Edge::Build);                                                   \
            REQUIRE(action.args.size() == 2);                                                      \
            REQUIRE(action.args.at(0).type == ArgType::BuildItemId);                               \
            REQUIRE(action.args.at(0).value == static_cast<size_t>(Building::Road));               \
            REQUIRE(action.args.at(1).type == ArgType::NodeId);                                    \
            const auto node_index = action.args.at(1).value;                                       \
            REQUIRE(g->roads().find(node_index) != g->roads().end());                              \
            REQUIRE(g->roads().at(node_index)->owner() == nullptr);                                \
        }                                                                                          \
        REQUIRE(num_buildable_roads_found == num_expected);                                        \
        exec_error(invalid_player_index,                                                           \
                   build(Building::Road, large_node_index),                                        \
                   ResType::InvalidPlayerId);                                                      \
        exec_error(player_index, { Edge::AcceptTrade, {} }, ResType::InvalidEdgeChoice);           \
        exec_error(player_index, { Edge::Build, {} }, ResType::InvalidNumberOfArgs);               \
        exec_error(player_index,                                                                   \
                   { Edge::Build, { { ArgType::NodeId, 4 }, { ArgType::NodeId, 4 } } },            \
                   ResType::InvalidArgumentType);                                                  \
        exec_error(player_index,                                                                   \
                   { Edge::Build,                                                                  \
                     { { ArgType::BuildItemId, static_cast<size_t>(Building::Road) },              \
                       { ArgType::BuildItemId, static_cast<size_t>(Building::Road) } } },          \
                   ResType::InvalidNodeId);                                                        \
        exec_error(player_index, build(Building::Road, large_node_index), ResType::InvalidNodeId); \
        exec_error(                                                                                \
            player_index, build(Building::Road, invalid_node_index), ResType::InvalidNodeId);      \
    };                                                                                             \
                                                                                                   \
    const auto check_offer_trade = [&](size_t current_player_index, size_t other_player_index) {   \
        const auto invalid_player_index = 100;                                                     \
        exec_error(current_player_index, { Edge::OfferTrade, {} }, ResType::InvalidTrade);         \
        exec_error(current_player_index,                                                           \
                   { Edge::OfferTrade, { { ArgType::NodeId, 0 } } },                               \
                   ResType::InvalidTrade);                                                         \
        exec_error(current_player_index,                                                           \
                   { Edge::OfferTrade,                                                             \
                     { { ArgType::GiveResourceType, static_cast<size_t>(Resource::Wheat) },        \
                       { ArgType::TakeResourceType, static_cast<size_t>(Resource::Sheep) } } },    \
                   ResType::InvalidTrade);                                                         \
        exec_error(current_player_index,                                                           \
                   { Edge::OfferTrade,                                                             \
                     { { ArgType::PlayerId, current_player_index },                                \
                       { ArgType::GiveResourceType, static_cast<size_t>(Resource::Wheat) },        \
                       { ArgType::TakeResourceType, static_cast<size_t>(Resource::Sheep) } } },    \
                   ResType::InvalidTrade);                                                         \
        exec_error(current_player_index,                                                           \
                   { Edge::OfferTrade,                                                             \
                     { { ArgType::PlayerId, invalid_player_index },                                \
                       { ArgType::GiveResourceType, static_cast<size_t>(Resource::Wheat) },        \
                       { ArgType::TakeResourceType, static_cast<size_t>(Resource::Sheep) } } },    \
                   ResType::InvalidTrade);                                                         \
        exec_error(current_player_index,                                                           \
                   { Edge::OfferTrade,                                                             \
                     { { ArgType::PlayerId, other_player_index },                                  \
                       { ArgType::TakeResourceType, static_cast<size_t>(Resource::Sheep) } } },    \
                   ResType::InvalidTrade);                                                         \
        exec_error(current_player_index,                                                           \
                   { Edge::OfferTrade,                                                             \
                     { { ArgType::PlayerId, other_player_index },                                  \
                       { ArgType::GiveResourceType, static_cast<size_t>(Resource::Sheep) } } },    \
                   ResType::InvalidTrade);                                                         \
        exec_error(current_player_index,                                                           \
                   { Edge::OfferTrade,                                                             \
                     { { ArgType::PlayerId, other_player_index },                                  \
                       { ArgType::GiveResourceType, static_cast<size_t>(Resource::Sheep) },        \
                       { ArgType::TakeResourceType, static_cast<size_t>(Resource::Sheep) } } },    \
                   ResType::InvalidTrade);                                                         \
        exec_error(current_player_index,                                                           \
                   { Edge::OfferTrade,                                                             \
                     { { ArgType::PlayerId, other_player_index },                                  \
                       { ArgType::GiveResourceType, static_cast<size_t>(Resource::Sheep) },        \
                       { ArgType::GiveResourceType, static_cast<size_t>(Resource::Sheep) },        \
                       { ArgType::GiveResourceType, static_cast<size_t>(Resource::Sheep) },        \
                       { ArgType::GiveResourceType, static_cast<size_t>(Resource::Sheep) },        \
                       { ArgType::GiveResourceType, static_cast<size_t>(Resource::Sheep) },        \
                       { ArgType::GiveResourceType, static_cast<size_t>(Resource::Sheep) },        \
                       { ArgType::GiveResourceType, static_cast<size_t>(Resource::Sheep) },        \
                       { ArgType::GiveResourceType, static_cast<size_t>(Resource::Sheep) },        \
                       { ArgType::GiveResourceType, static_cast<size_t>(Resource::Sheep) },        \
                       { ArgType::TakeResourceType, static_cast<size_t>(Resource::Wheat) } } },    \
                   ResType::CannotAfford);                                                         \
    };                                                                                             \
                                                                                                   \
    const auto check_choose_initial_resources = [&](size_t player_index) {                         \
        const auto actions = g->players().at(player_index)->get_available_actions();               \
        REQUIRE(actions.size() == 2);                                                              \
        for (const auto& action : actions) {                                                       \
            REQUIRE(action.edge == Edge::ChooseInitialResources);                                  \
            REQUIRE(action.args.size() == 1);                                                      \
            REQUIRE(action.args.at(0).type == ArgType::NodeId);                                    \
            const auto node_index = action.args.at(0).value;                                       \
            REQUIRE(g->junctions().find(node_index) != g->junctions().end());                      \
            const auto junction = g->junctions().at(node_index);                                   \
            REQUIRE(junction->owner() == g->players().at(player_index));                           \
            const auto settlements = g->players().at(player_index)->settlements();                 \
            REQUIRE(std::find(settlements.begin(), settlements.end(), junction)                    \
                    != settlements.end());                                                         \
        }                                                                                          \
    };                                                                                             \
                                                                                                   \
    const auto check_single_action = [&](size_t player_index, const Edge& edge) {                  \
        const auto actions = g->players().at(player_index)->get_available_actions();               \
        REQUIRE(actions.size() == 1);                                                              \
        for (const auto& action : actions) {                                                       \
            REQUIRE(action.edge == edge);                                                          \
            REQUIRE(action.args.empty());                                                          \
        }                                                                                          \
    };                                                                                             \
                                                                                                   \
    const auto check_to_root = [&](size_t player_index) {                                          \
        check_single_action(player_index, Edge::ToRoot);                                           \
    };                                                                                             \
                                                                                                   \
    const auto check_end_turn = [&](size_t player_index) {                                         \
        const auto actions = g->players().at(player_index)->get_available_actions();               \
        bool found = false;                                                                        \
        for (const auto& action : actions) {                                                       \
            if (action.edge == Edge::EndTurn) {                                                    \
                found = true;                                                                      \
                break;                                                                             \
            }                                                                                      \
        }                                                                                          \
        REQUIRE(found == true);                                                                    \
    };                                                                                             \
                                                                                                   \
    const auto check_roll_dice = [&](size_t player_index) {                                        \
        check_single_action(player_index, Edge::RollDice);                                         \
        exec_error(player_index,                                                                   \
                   { Edge::RollDice, { { ArgType::DiceRoll, 0 } } },                               \
                   ResType::DiceRollOutOfRange);                                                   \
        exec_error(player_index,                                                                   \
                   { Edge::RollDice, { { ArgType::DiceRoll, 1 } } },                               \
                   ResType::DiceRollOutOfRange);                                                   \
        exec_error(player_index,                                                                   \
                   { Edge::RollDice, { { ArgType::DiceRoll, 13 } } },                              \
                   ResType::DiceRollOutOfRange);                                                   \
    };                                                                                             \
                                                                                                   \
    const auto check_settlements = [&]() {                                                         \
        for (const auto& j_it : g->junctions()) {                                                  \
            const auto& j = j_it.second;                                                           \
            const auto& map_it = settlements.find(j->index());                                     \
            if (map_it == settlements.end()) {                                                     \
                REQUIRE(j->is_settleable() == true);                                               \
                REQUIRE(j->owner() == nullptr);                                                    \
            } else {                                                                               \
                REQUIRE(j->is_settleable() == false);                                              \
                const auto& player_index = settlements.at(j->index());                             \
                if (player_index == -1) {                                                          \
                    REQUIRE(j->owner() == nullptr);                                                \
                } else {                                                                           \
                    REQUIRE(j->owner() != nullptr);                                                \
                    REQUIRE(j->owner()->index() == player_index);                                  \
                }                                                                                  \
            }                                                                                      \
        }                                                                                          \
    };                                                                                             \
                                                                                                   \
    const auto check_roads = [&]() {                                                               \
        for (const auto& r_it : g->roads()) {                                                      \
            const auto& r = r_it.second;                                                           \
            const auto& map_it = roads.find(r->index());                                           \
            if (map_it == roads.end()) {                                                           \
                REQUIRE(r->owner() == nullptr);                                                    \
            } else {                                                                               \
                const auto& player_index = roads.at(r->index());                                   \
                REQUIRE(r->owner()->index() == player_index);                                      \
            }                                                                                      \
        }                                                                                          \
    };                                                                                             \
                                                                                                   \
    const auto dump_actions = [&]() {                                                              \
        std::cout << *g << std::endl;                                                              \
        for (const auto& player : g->players()) {                                                  \
            std::cout << *player << std::endl;                                                     \
            for (const auto& action : player->get_available_actions()) {                           \
                std::cout << " - " << action << std::endl;                                         \
            }                                                                                      \
        }                                                                                          \
        std::cout << std::endl;                                                                    \
    };                                                                                             \
                                                                                                   \
    const auto do_first_two_rounds_standard_3p = [&]() {                                           \
        gs.robber_location = 143;                                                                  \
        exec_ok(0, build(Building::Settlement, 4));                                                \
        exec_ok(0, build(Building::Road, 7));                                                      \
        exec_ok(1, { Edge::ToRoot, {} });                                                          \
        exec_ok(1, build(Building::Settlement, 5));                                                \
        exec_ok(1, build(Building::Road, 9));                                                      \
        exec_ok(2, { Edge::ToRoot, {} });                                                          \
        exec_ok(2, build(Building::Settlement, 90));                                               \
        exec_ok(2, build(Building::Road, 80));                                                     \
        exec_ok(2, { Edge::ToRoot, {} });                                                          \
        exec_ok(2, build(Building::Settlement, 91));                                               \
        exec_ok(2, build(Building::Road, 63));                                                     \
        exec_ok(2, { Edge::ChooseInitialResources, { { ArgType::NodeId, 91 } } });                 \
        exec_ok(1, { Edge::ToRoot, {} });                                                          \
        exec_ok(1, build(Building::Settlement, 6));                                                \
        exec_ok(1, build(Building::Road, 10));                                                     \
        exec_ok(1, { Edge::ChooseInitialResources, { { ArgType::NodeId, 6 } } });                  \
        exec_ok(0, { Edge::ToRoot, {} });                                                          \
        exec_ok(0, build(Building::Settlement, 26));                                               \
        exec_ok(0, build(Building::Road, 18));                                                     \
        exec_ok(0, { Edge::ChooseInitialResources, { { ArgType::NodeId, 26 } } });                 \
                                                                                                   \
        gs.is_first_round = false;                                                                 \
        gs.is_second_round = false;                                                                \
        gs.roads_built = 6;                                                                        \
        gs.settlements_built = 6;                                                                  \
        gs.turn = 6;                                                                               \
        gs.round = 2;                                                                              \
        ps[0].vertex = Vertex::WaitForTurn;                                                        \
        ps[0].num_resources = 2;                                                                   \
        ps[0].is_current_player = true;                                                            \
        ps[0].settlements = 2;                                                                     \
        ps[0].roads = 2;                                                                           \
        ps[0].public_victory_points = 2;                                                           \
        ps[1].vertex = Vertex::WaitForTurn;                                                        \
        ps[1].num_resources = 1;                                                                   \
        ps[1].settlements = 2;                                                                     \
        ps[1].roads = 2;                                                                           \
        ps[1].public_victory_points = 2;                                                           \
        ps[2].vertex = Vertex::WaitForTurn;                                                        \
        ps[2].num_resources = 3;                                                                   \
        ps[2].settlements = 2;                                                                     \
        ps[2].roads = 2;                                                                           \
        ps[2].public_victory_points = 2;                                                           \
        roads[7] = 0;                                                                              \
        roads[9] = 1;                                                                              \
        roads[10] = 1;                                                                             \
        roads[18] = 0;                                                                             \
        roads[63] = 2;                                                                             \
        roads[80] = 2;                                                                             \
        settlements[4] = 0;                                                                        \
        settlements[5] = 1;                                                                        \
        settlements[6] = 1;                                                                        \
        settlements[13] = -1;                                                                      \
        settlements[14] = -1;                                                                      \
        settlements[15] = -1;                                                                      \
        settlements[16] = -1;                                                                      \
        settlements[26] = 0;                                                                       \
        settlements[38] = -1;                                                                      \
        settlements[39] = -1;                                                                      \
        settlements[71] = -1;                                                                      \
        settlements[72] = -1;                                                                      \
        settlements[90] = 2;                                                                       \
        settlements[91] = 2;                                                                       \
        settlements[105] = -1;                                                                     \
        settlements[106] = -1;                                                                     \
        settlements[107] = -1;                                                                     \
        check_state();                                                                             \
        check_to_root(0);                                                                          \
        check_no_actions(1);                                                                       \
        check_no_actions(2);                                                                       \
    };

Action build(Building b, size_t node_index)
{
    return { Edge::Build,
             { { ArgType::BuildItemId, static_cast<size_t>(b) },
               { ArgType::NodeId, node_index } } };
}

Action
trade(const std::vector<size_t>& players, const ResourceCounts& from, const ResourceCounts& to)
{
    std::vector<ActionArgument> args;
    args.reserve(players.size() + 10);
    for (const size_t player_index : players) {
        args.push_back({ ArgType::PlayerId, player_index });
    }
    for (const auto from_it : from) {
        for (size_t i = 0; i < from_it.second; ++i) {
            args.push_back({ ArgType::GiveResourceType, static_cast<size_t>(from_it.first) });
        }
    }
    for (const auto to_it : to) {
        for (size_t i = 0; i < to_it.second; ++i) {
            args.push_back({ ArgType::TakeResourceType, static_cast<size_t>(to_it.first) });
        }
    }
    return { Edge::OfferTrade, args };
}

Scenario_ get_single_scenario()
{
    return Scenario_(
        1,
        2,
        3,
        6,
        { {
              Building::City,
              { { Resource::Ore, 3 }, { Resource::Wheat, 2 } },
          },
          {
              Building::DevelopmentCard,
              { { Resource::Ore, 1 }, { Resource::Sheep, 1 }, { Resource::Wheat, 1 } },
          },
          {
              Building::Road,
              {
                  { Resource::Brick, 1 },
                  { Resource::Wood, 1 },
              },
          },
          {
              Building::Settlement,
              { { Resource::Brick, 1 },
                { Resource::Sheep, 1 },
                { Resource::Wheat, 1 },
                { Resource::Wood, 1 } },
          } },
        {
            { Building::DevelopmentCard, 25 },
        },
        {
            { Building::City, 2 },
            { Building::Road, 6 },
            { Building::Settlement, 2 },
        },
        {
            { DevelopmentCard::Knight, 14 },
            { DevelopmentCard::Monopoly, 2 },
            { DevelopmentCard::RoadBuilding, 2 },
            { DevelopmentCard::VictoryPoint, 5 },
            { DevelopmentCard::YearOfPlenty, 2 },
        },
        {
            { NonYieldingResource::Desert, 1 },
        },
        {},
        {
            { { Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat, Resource::Wood },
              3 },
            { { Resource::Wheat }, 2 },
        });
}

Parameters get_single_parameters()
{
    return Parameters{
        IterationType::Fixed, IterationType::Fixed, IterationType::Fixed, IterationType::Fixed, 1, 3
    };
}

Scenario_ get_triple_scenario()
{
    return Scenario_(
        1,
        3,
        3,
        6,
        { {
              Building::City,
              { { Resource::Ore, 3 }, { Resource::Wheat, 2 } },
          },
          {
              Building::DevelopmentCard,
              { { Resource::Ore, 1 }, { Resource::Sheep, 1 }, { Resource::Wheat, 1 } },
          },
          {
              Building::Road,
              {
                  { Resource::Brick, 1 },
                  { Resource::Wood, 1 },
              },
          },
          {
              Building::Settlement,
              { { Resource::Brick, 1 },
                { Resource::Sheep, 1 },
                { Resource::Wheat, 1 },
                { Resource::Wood, 1 } },
          } },
        {
            { Building::DevelopmentCard, 25 },
        },
        {
            { Building::City, 3 },
            { Building::Road, 10 },
            { Building::Settlement, 4 },
        },
        {
            { DevelopmentCard::Knight, 14 },
            { DevelopmentCard::Monopoly, 2 },
            { DevelopmentCard::RoadBuilding, 2 },
            { DevelopmentCard::VictoryPoint, 5 },
            { DevelopmentCard::YearOfPlenty, 2 },
        },
        {
            { Resource::Brick, 1 },
            { Resource::Wood, 1 },
            { NonYieldingResource::Desert, 1 },
        },
        { 6, 9 },
        {
            { { Resource::Brick }, 2 },
            { { Resource::Wood }, 2 },
            { { Resource::Wheat }, 2 },
            { { Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat, Resource::Wood },
              3 },
        });
}

Parameters get_triple_parameters(size_t num_players)
{
    return Parameters{ IterationType::Fixed, IterationType::Fixed, IterationType::Fixed,
                       IterationType::Fixed, num_players,          3 };
}

Scenario_ get_standard_scenario()
{
    return Scenario_(
        2,
        5,
        8,
        12,
        { {
              Building::City,
              { { Resource::Ore, 3 }, { Resource::Wheat, 2 } },
          },
          {
              Building::DevelopmentCard,
              { { Resource::Ore, 1 }, { Resource::Sheep, 1 }, { Resource::Wheat, 1 } },
          },
          {
              Building::Road,
              {
                  { Resource::Brick, 1 },
                  { Resource::Wood, 1 },
              },
          },
          {
              Building::Settlement,
              { { Resource::Brick, 1 },
                { Resource::Sheep, 1 },
                { Resource::Wheat, 1 },
                { Resource::Wood, 1 } },
          } },
        {
            { Building::DevelopmentCard, 25 },
        },
        {
            { Building::City, 4 },
            { Building::Road, 15 },
            { Building::Settlement, 5 },
        },
        {
            { DevelopmentCard::Knight, 14 },
            { DevelopmentCard::Monopoly, 2 },
            { DevelopmentCard::RoadBuilding, 2 },
            { DevelopmentCard::VictoryPoint, 5 },
            { DevelopmentCard::YearOfPlenty, 2 },
        },
        { { Resource::Brick, 3 },
          { Resource::Ore, 3 },
          { Resource::Sheep, 4 },
          { Resource::Wheat, 4 },
          { Resource::Wood, 4 },
          { NonYieldingResource::Desert, 1 } },
        { 2, 3, 3, 4, 4, 5, 5, 6, 6, 8, 8, 9, 9, 10, 10, 11, 11, 12 },
        {
            { { Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat, Resource::Wood },
              3 },
            { { Resource::Wheat }, 2 },
            { {
                  Resource::Ore,
              },
              2 },
            { { Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat, Resource::Wood },
              3 },
            { { Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat, Resource::Wood },
              3 },
            { {
                  Resource::Sheep,
              },
              2 },
            { { Resource::Brick, Resource::Ore, Resource::Sheep, Resource::Wheat, Resource::Wood },
              3 },
            { {
                  Resource::Brick,
              },
              2 },
            { {
                  Resource::Wood,
              },
              2 },
        });
}

Parameters get_standard_parameters(size_t num_players)
{
    return Parameters{ IterationType::Fixed, IterationType::Fixed, IterationType::Fixed,
                       IterationType::Fixed, num_players,          10 };
}

// NOLINTNEXTLINE(readability-function-size,google-readability-function-size)
TEST_CASE("Single board first two rounds", "[Game] [Game.Single]")
{
    SECTION("One player")
    {
        auto b = Board::from_file("static/boards/Single.board");
        auto s = get_single_scenario();
        auto p = get_single_parameters();
        auto g = Game::initialize(&b, s, p);

        bootstrap_tests();

        REQUIRE(g->hexes().size() == 1);
        REQUIRE(g->junctions().size() == 6);
        REQUIRE(g->roads().size() == 6);
        REQUIRE(g->players().size() == 1);

        gs.robber_location = 9;
        ps[0].is_current_player = true;

        check_state();
        check_build_settlement(0, 6);

        exec_ok(0, build(Building::Settlement, 2));
        settlements[2] = 0;
        settlements[5] = -1;
        settlements[6] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 1;
        ps[0].public_victory_points = 1;
        check_state();
        check_build_road(0, 2);

        exec_ok(0, build(Building::Road, 4));
        roads[4] = 0;
        check_roads();

        gs.roads_built += 1;
        gs.is_first_round = false;
        gs.is_second_round = true;
        gs.round += 1;
        gs.turn += 1;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].roads = 1;
        check_state();
        check_to_root(0);

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check_state();
        check_build_settlement(0, 3);

        exec_ok(0, build(Building::Settlement, 12));
        settlements[12] = 0;
        settlements[16] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 2;
        ps[0].public_victory_points = 2;
        check_state();
        check_build_road(0, 4);

        exec_ok(0, build(Building::Road, 10));
        roads[10] = 0;
        check_roads();

        gs.roads_built += 1;
        ps[0].vertex = Vertex::ChooseInitialResources;
        ps[0].roads = 2;
        check_state();
        check_choose_initial_resources(0);

        exec_ok(0, { Edge::ChooseInitialResources, { { ArgType::NodeId, 12 } } });

        gs.is_second_round = false;
        gs.round += 1;
        gs.turn += 1;
        ps[0].vertex = Vertex::WaitForTurn;
        check_state();
        check_to_root(0);

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check_state();
        check_roll_dice(0);

        exec_ok(0, { Edge::RollDice, { { ArgType::DiceRoll, 12 } } });

        gs.has_rolled = true;
        gs.dice_total = 12;
        check_state();
        check_end_turn(0);

        // dump_actions();

        delete g;
    }
}

// NOLINTNEXTLINE(readability-function-size,google-readability-function-size)
TEST_CASE("Triple board first two rounds", "[Game] [Game.Triple]")
{
    SECTION("One player")
    {
        auto b = Board::from_file("static/boards/Triple.board");
        auto s = get_triple_scenario();
        auto p = get_triple_parameters(1);
        auto g = Game::initialize(&b, s, p);

        bootstrap_tests();

        REQUIRE(g->hexes().size() == 3);
        REQUIRE(g->junctions().size() == 13);
        REQUIRE(g->roads().size() == 15);
        REQUIRE(g->players().size() == 1);

        gs.robber_location = 30;
        ps[0].is_current_player = true;

        check_state();
        check_build_settlement(0, 13);

        exec_ok(0, build(Building::Settlement, 20));
        settlements[20] = 0;
        settlements[10] = -1;
        settlements[26] = -1;
        settlements[27] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 1;
        ps[0].public_victory_points = 1;
        check_state();
        check_build_road(0, 3);

        exec_ok(0, build(Building::Road, 15));
        roads[15] = 0;
        check_roads();

        gs.roads_built += 1;
        gs.is_first_round = false;
        gs.is_second_round = true;
        gs.round += 1;
        gs.turn += 1;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].roads = 1;
        check_state();
        check_to_root(0);

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check_state();
        check_build_settlement(0, 9);

        exec_ok(0, build(Building::Settlement, 3));
        settlements[3] = 0;
        settlements[9] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 2;
        ps[0].public_victory_points = 2;
        check_state();
        check_build_road(0, 5);

        exec_ok(0, build(Building::Road, 7));
        roads[7] = 0;
        check_roads();

        gs.roads_built += 1;
        ps[0].vertex = Vertex::ChooseInitialResources;
        ps[0].roads = 2;
        check_state();
        check_choose_initial_resources(0);

        exec_ok(0, { Edge::ChooseInitialResources, { { ArgType::NodeId, 3 } } });

        gs.is_second_round = false;
        gs.round += 1;
        gs.turn += 1;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].num_resources = 1;
        check_state();
        check_to_root(0);

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check_state();
        check_roll_dice(0);

        exec_ok(0, { Edge::RollDice, { { ArgType::DiceRoll, 6 } } });

        gs.has_rolled = true;
        gs.dice_total = 6;
        ps[0].num_resources += 2;
        check_state();
        check_end_turn(0);

        // dump_actions();

        delete g;
    }

    SECTION("Two players")
    {
        auto b = Board::from_file("static/boards/Triple.board");
        auto s = get_triple_scenario();
        auto p = get_triple_parameters(2);
        auto g = Game::initialize(&b, s, p);

        bootstrap_tests();

        REQUIRE(g->hexes().size() == 3);
        REQUIRE(g->junctions().size() == 13);
        REQUIRE(g->roads().size() == 15);
        REQUIRE(g->players().size() == 2);

        gs.robber_location = 30;
        ps[0].is_current_player = true;
        ps[1].vertex = Vertex::WaitForTurn;

        check_state();
        check_build_settlement(0, 13);
        check_no_actions(1);

        exec_error(1, build(Building::Settlement, 20), ResType::InvalidEdgeChoice);
        exec_ok(0, build(Building::Settlement, 20));
        settlements[20] = 0;
        settlements[10] = -1;
        settlements[26] = -1;
        settlements[27] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 1;
        ps[0].public_victory_points = 1;
        check_state();
        check_build_road(0, 3);
        check_no_actions(1);

        exec_error(1, build(Building::Settlement, 20), ResType::InvalidEdgeChoice);
        exec_ok(0, build(Building::Road, 15));
        roads[15] = 0;
        check_roads();

        gs.roads_built += 1;
        gs.turn += 1;
        ps[0].is_current_player = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].roads = 1;
        ps[1].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_to_root(1);

        exec_error(0, { Edge::ToRoot, {} }, ResType::InvalidEdgeChoice);
        exec_ok(1, { Edge::ToRoot, {} });

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_build_settlement(1, 9);

        exec_ok(1, build(Building::Settlement, 3));
        settlements[3] = 1;
        settlements[9] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 1;
        ps[1].public_victory_points = 1;
        check_state();
        check_no_actions(0);
        check_build_road(1, 2);

        exec_error(0, build(Building::Settlement, 4), ResType::InvalidEdgeChoice);
        exec_ok(1, build(Building::Road, 6));
        roads[6] = 1;
        check_roads();

        gs.roads_built += 1;
        gs.is_first_round = false;
        gs.is_second_round = true;
        gs.round += 1;
        gs.turn += 1;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].roads = 1;
        check_state();
        check_no_actions(0);
        check_to_root(1);

        exec_error(0, { Edge::ToRoot, {} }, ResType::InvalidEdgeChoice);
        exec_ok(1, { Edge::ToRoot, {} });

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_build_settlement(1, 7);

        exec_ok(1, build(Building::Settlement, 33));
        settlements[33] = 1;
        settlements[37] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 2;
        ps[1].public_victory_points = 2;
        check_state();
        check_no_actions(0);
        check_build_road(1, 4);

        exec_ok(1, build(Building::Road, 29));
        roads[29] = 1;
        check_roads();

        gs.roads_built += 1;
        ps[1].vertex = Vertex::ChooseInitialResources;
        ps[1].roads = 2;
        check_state();
        check_no_actions(0);
        check_choose_initial_resources(1);

        exec_ok(1, { Edge::ChooseInitialResources, { { ArgType::NodeId, 3 } } });

        gs.turn += 1;
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
        settlements[34] = 0;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 2;
        ps[0].public_victory_points = 2;
        check_state();
        check_build_road(0, 5);
        check_no_actions(1);

        exec_ok(0, build(Building::Road, 7));
        roads[7] = 0;
        check_roads();

        gs.roads_built += 1;
        ps[0].vertex = Vertex::ChooseInitialResources;
        ps[0].roads = 2;
        check_state();
        check_choose_initial_resources(0);
        check_no_actions(1);

        exec_ok(0, { Edge::ChooseInitialResources, { { ArgType::NodeId, 20 } } });

        gs.is_second_round = false;
        gs.turn += 1;
        gs.round += 1;
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

        exec_ok(0, { Edge::RollDice, { { ArgType::DiceRoll, 6 } } });

        gs.has_rolled = true;
        gs.dice_total = 6;
        ps[0].num_resources += 1;
        ps[1].num_resources += 1;
        check_state();
        check_end_turn(0);

        // dump_actions();

        delete g;
    }

    SECTION("Three players")
    {
        auto b = Board::from_file("static/boards/Triple.board");
        auto s = get_triple_scenario();
        auto p = get_triple_parameters(3);
        auto g = Game::initialize(&b, s, p);

        bootstrap_tests();

        REQUIRE(g->hexes().size() == 3);
        REQUIRE(g->junctions().size() == 13);
        REQUIRE(g->roads().size() == 15);
        REQUIRE(g->players().size() == 3);

        gs.robber_location = 30;
        ps[0].is_current_player = true;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[2].vertex = Vertex::WaitForTurn;

        check_state();
        check_build_settlement(0, 13);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, build(Building::Settlement, 3));
        settlements[3] = 0;
        settlements[9] = -1;
        settlements[10] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 1;
        ps[0].public_victory_points = 1;
        check_state();
        check_build_road(0, 2);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, build(Building::Road, 5));
        roads[5] = 0;
        check_roads();

        gs.roads_built += 1;
        gs.turn += 1;
        ps[0].is_current_player = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].roads = 1;
        ps[1].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_to_root(1);
        check_no_actions(2);

        exec_ok(1, { Edge::ToRoot, {} });

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_build_settlement(1, 10);

        exec_ok(1, build(Building::Settlement, 4));
        settlements[4] = 1;
        settlements[11] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 1;
        ps[1].public_victory_points = 1;
        check_state();
        check_no_actions(0);
        check_build_road(1, 2);
        check_no_actions(2);

        exec_ok(1, build(Building::Road, 7));
        roads[7] = 1;
        check_roads();

        gs.roads_built += 1;
        gs.turn += 1;
        ps[1].is_current_player = false;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].roads = 1;
        ps[2].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_to_root(2);

        exec_ok(2, { Edge::ToRoot, {} });

        ps[2].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_build_settlement(2, 8);

        exec_ok(2, build(Building::Settlement, 19));
        settlements[19] = 2;
        settlements[26] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[2].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[2].settlements = 1;
        ps[2].public_victory_points = 1;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_build_road(2, 2);

        exec_ok(2, build(Building::Road, 13));
        roads[13] = 2;
        check_roads();

        gs.roads_built += 1;
        gs.is_first_round = false;
        gs.is_second_round = true;
        gs.turn += 1;
        gs.round += 1;
        ps[2].vertex = Vertex::WaitForTurn;
        ps[2].roads = 1;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_to_root(2);

        exec_ok(2, { Edge::ToRoot, {} });

        ps[2].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_build_settlement(2, 6);

        exec_ok(2, build(Building::Settlement, 20));
        settlements[20] = 2;
        settlements[27] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[2].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[2].settlements = 2;
        ps[2].public_victory_points = 2;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_build_road(2, 4);

        exec_ok(2, build(Building::Road, 15));
        roads[15] = 2;
        check_roads();

        gs.roads_built += 1;
        ps[2].vertex = Vertex::ChooseInitialResources;
        ps[2].roads = 2;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_choose_initial_resources(2);

        exec_ok(2, { Edge::ChooseInitialResources, { { ArgType::NodeId, 20 } } });

        gs.turn += 1;
        ps[1].is_current_player = true;
        ps[2].vertex = Vertex::WaitForTurn;
        ps[2].is_current_player = false;
        ps[2].num_resources = 2;
        check_state();
        check_no_actions(0);
        check_to_root(1);
        check_no_actions(2);

        exec_ok(1, { Edge::ToRoot, {} });

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_build_settlement(1, 4);
        check_no_actions(2);

        exec_ok(1, build(Building::Settlement, 21));
        settlements[21] = 1;
        check_settlements();

        gs.settlements_built += 1;
        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 2;
        ps[1].public_victory_points = 2;
        check_state();
        check_no_actions(0);
        check_build_road(1, 4);
        check_no_actions(2);

        exec_ok(1, build(Building::Road, 17));
        roads[17] = 1;
        check_roads();

        gs.roads_built += 1;
        ps[1].vertex = Vertex::ChooseInitialResources;
        ps[1].roads = 2;
        check_state();
        check_no_actions(0);
        check_choose_initial_resources(1);
        check_no_actions(2);

        exec_ok(1, { Edge::ChooseInitialResources, { { ArgType::NodeId, 4 } } });

        gs.turn += 1;
        ps[0].is_current_player = true;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].is_current_player = false;
        ps[1].num_resources = 1;
        check_state();
        check_to_root(0);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check_state();
        check_build_settlement(0, 3);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, build(Building::Settlement, 37));
        settlements[37] = 0;
        settlements[33] = -1;
        settlements[34] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 2;
        ps[0].public_victory_points = 2;
        check_state();
        check_build_road(0, 3);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, build(Building::Road, 6));
        roads[6] = 0;
        check_roads();

        gs.roads_built += 1;
        ps[0].vertex = Vertex::ChooseInitialResources;
        ps[0].roads = 2;
        check_state();
        check_choose_initial_resources(0);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, { Edge::ChooseInitialResources, { { ArgType::NodeId, 37 } } });

        gs.is_second_round = false;
        gs.turn += 1;
        gs.round += 1;
        ps[0].vertex = Vertex::WaitForTurn;
        check_state();
        check_to_root(0);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check_state();
        check_roll_dice(0);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, { Edge::RollDice, { { ArgType::DiceRoll, 6 } } });

        gs.has_rolled = true;
        gs.dice_total = 6;
        ps[0].num_resources += 1;
        ps[2].num_resources += 2;
        check_state();
        check_end_turn(0);

        // dump_actions();

        delete g;
    }
}

// NOLINTNEXTLINE(readability-function-size,google-readability-function-size)
TEST_CASE("Standard board first two rounds", "[Game] [Game.Standard]")
{
    SECTION("Two players")
    {
        auto b = Board::from_file("static/boards/Standard.board");
        auto s = get_standard_scenario();
        auto p = get_standard_parameters(2);
        auto g = Game::initialize(&b, s, p);

        bootstrap_tests();

        REQUIRE(g->hexes().size() == 19);
        REQUIRE(g->junctions().size() == 54);
        REQUIRE(g->roads().size() == 72);
        REQUIRE(g->players().size() == 2);

        gs.robber_location = 143;
        ps[0].is_current_player = true;
        ps[1].vertex = Vertex::WaitForTurn;

        check_state();
        check_build_settlement(0, 54);
        check_no_actions(1);

        exec_ok(0, build(Building::Settlement, 4));
        settlements[4] = 0;
        settlements[13] = -1;
        settlements[14] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 1;
        ps[0].public_victory_points = 1;
        check_state();
        check_build_road(0, 2);
        check_no_actions(1);

        exec_ok(0, build(Building::Road, 7));
        roads[7] = 0;
        check_roads();

        gs.roads_built += 1;
        gs.turn += 1;
        ps[0].is_current_player = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].roads = 1;
        ps[1].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_to_root(1);

        exec_ok(1, { Edge::ToRoot, {} });

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_build_settlement(1, 51);

        exec_ok(1, build(Building::Settlement, 5));
        settlements[5] = 1;
        settlements[15] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 1;
        ps[1].public_victory_points = 1;
        check_state();
        check_no_actions(0);
        check_build_road(1, 2);

        exec_ok(1, build(Building::Road, 9));
        roads[9] = 1;
        check_roads();

        gs.roads_built += 1;
        gs.turn += 1;
        gs.round += 1;
        gs.is_first_round = false;
        gs.is_second_round = true;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].roads = 1;
        check_state();
        check_no_actions(0);
        check_to_root(1);

        exec_ok(1, { Edge::ToRoot, {} });

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_build_settlement(1, 49);

        exec_ok(1, build(Building::Settlement, 6));
        settlements[6] = 1;
        settlements[16] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 2;
        ps[1].public_victory_points = 2;
        check_state();
        check_no_actions(0);
        check_build_road(1, 5);

        exec_ok(1, build(Building::Road, 10));
        roads[10] = 1;
        check_roads();

        gs.roads_built += 1;
        ps[1].vertex = Vertex::ChooseInitialResources;
        ps[1].roads = 2;
        check_state();
        check_no_actions(0);
        check_choose_initial_resources(1);

        exec_ok(1, { Edge::ChooseInitialResources, { { ArgType::NodeId, 5 } } });

        gs.turn += 1;
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
        check_build_settlement(0, 47);
        check_no_actions(1);

        exec_ok(0, build(Building::Settlement, 26));
        settlements[26] = 0;
        settlements[38] = -1;
        settlements[39] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 2;
        ps[0].public_victory_points = 2;
        check_state();
        check_build_road(0, 4);
        check_no_actions(1);

        exec_ok(0, build(Building::Road, 18));
        roads[18] = 0;
        check_roads();

        gs.roads_built += 1;
        ps[0].vertex = Vertex::ChooseInitialResources;
        ps[0].roads = 2;
        check_state();
        check_choose_initial_resources(0);
        check_no_actions(1);

        exec_ok(0, { Edge::ChooseInitialResources, { { ArgType::NodeId, 26 } } });

        gs.is_second_round = false;
        gs.turn += 1;
        gs.round += 1;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].num_resources = 2;
        check_state();
        check_to_root(0);
        check_no_actions(1);

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check_state();
        check_roll_dice(0);
        check_no_actions(1);

        exec_ok(0, { Edge::RollDice, { { ArgType::DiceRoll, 6 } } });

        gs.has_rolled = true;
        gs.dice_total = 6;
        check_state();
        check_end_turn(0);
        check_no_actions(1);

        exec_ok(0, { Edge::EndTurn, {} });

        gs.turn += 1;
        gs.has_rolled = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].is_current_player = false;
        ps[1].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_to_root(1);

        exec_ok(1, { Edge::ToRoot, {} });

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_roll_dice(1);

        // dump_actions();

        delete g;
    }

    SECTION("Three players")
    {
        auto b = Board::from_file("static/boards/Standard.board");
        auto s = get_standard_scenario();
        auto p = get_standard_parameters(3);
        auto g = Game::initialize(&b, s, p);

        bootstrap_tests();

        REQUIRE(g->hexes().size() == 19);
        REQUIRE(g->junctions().size() == 54);
        REQUIRE(g->roads().size() == 72);
        REQUIRE(g->players().size() == 3);

        gs.robber_location = 143;
        ps[0].is_current_player = true;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[2].vertex = Vertex::WaitForTurn;

        check_state();
        check_build_settlement(0, 54);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, build(Building::Settlement, 4));
        settlements[4] = 0;
        settlements[13] = -1;
        settlements[14] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 1;
        ps[0].public_victory_points = 1;
        check_state();
        check_build_road(0, 2);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, build(Building::Road, 7));
        roads[7] = 0;
        check_roads();

        gs.roads_built += 1;
        gs.turn += 1;
        ps[0].is_current_player = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].roads = 1;
        ps[1].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_to_root(1);
        check_no_actions(2);

        exec_ok(1, { Edge::ToRoot, {} });

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_build_settlement(1, 51);
        check_no_actions(2);

        exec_ok(1, build(Building::Settlement, 5));
        settlements[5] = 1;
        settlements[15] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 1;
        ps[1].public_victory_points = 1;
        check_state();
        check_no_actions(0);
        check_build_road(1, 2);
        check_no_actions(2);

        exec_ok(1, build(Building::Road, 9));
        roads[9] = 1;
        check_roads();

        gs.roads_built += 1;
        gs.turn += 1;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].roads = 1;
        ps[1].is_current_player = false;
        ps[2].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_to_root(2);

        exec_ok(2, { Edge::ToRoot, {} });

        ps[2].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_build_settlement(2, 49);

        exec_ok(2, build(Building::Settlement, 90));
        settlements[90] = 2;
        settlements[71] = -1;
        settlements[105] = -1;
        settlements[106] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[2].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[2].settlements = 1;
        ps[2].public_victory_points = 1;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_build_road(2, 3);

        exec_ok(2, build(Building::Road, 80));
        roads[80] = 2;
        check_roads();

        gs.roads_built += 1;
        gs.turn += 1;
        gs.round += 1;
        gs.is_first_round = false;
        gs.is_second_round = true;
        ps[2].vertex = Vertex::WaitForTurn;
        ps[2].roads = 1;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_to_root(2);

        exec_ok(2, { Edge::ToRoot, {} });

        ps[2].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_build_settlement(2, 45);

        exec_ok(2, build(Building::Settlement, 91));
        settlements[91] = 2;
        settlements[72] = -1;
        settlements[107] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[2].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[2].settlements = 2;
        ps[2].public_victory_points = 2;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_build_road(2, 7);

        exec_ok(2, build(Building::Road, 63));
        roads[63] = 2;
        check_roads();

        gs.roads_built += 1;
        ps[2].vertex = Vertex::ChooseInitialResources;
        ps[2].roads = 2;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_choose_initial_resources(2);

        exec_ok(2, { Edge::ChooseInitialResources, { { ArgType::NodeId, 91 } } });

        gs.turn += 1;
        ps[1].is_current_player = true;
        ps[2].is_current_player = false;
        ps[2].vertex = Vertex::WaitForTurn;
        ps[2].roads = 2;
        ps[2].num_resources = 3;
        check_state();
        check_no_actions(0);
        check_to_root(1);
        check_no_actions(2);

        exec_ok(1, { Edge::ToRoot, {} });

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_build_settlement(1, 42);
        check_no_actions(2);

        exec_ok(1, build(Building::Settlement, 6));
        settlements[6] = 1;
        settlements[16] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[1].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[1].settlements = 2;
        ps[1].public_victory_points = 2;
        check_state();
        check_no_actions(0);
        check_build_road(1, 5);
        check_no_actions(2);

        exec_ok(1, build(Building::Road, 10));
        roads[10] = 1;
        check_roads();

        gs.roads_built += 1;
        ps[1].vertex = Vertex::ChooseInitialResources;
        ps[1].roads = 2;
        check_state();
        check_no_actions(0);
        check_choose_initial_resources(1);
        check_no_actions(2);

        exec_ok(1, { Edge::ChooseInitialResources, { { ArgType::NodeId, 6 } } });

        gs.turn += 1;
        ps[0].is_current_player = true;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].is_current_player = false;
        ps[1].num_resources = 1;
        check_state();
        check_to_root(0);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check_state();
        check_build_settlement(0, 40);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, build(Building::Settlement, 26));
        settlements[26] = 0;
        settlements[38] = -1;
        settlements[39] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[0].vertex = Vertex::AfterBuildingFreeSettlement;
        ps[0].settlements = 2;
        ps[0].public_victory_points = 2;
        check_state();
        check_build_road(0, 4);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, build(Building::Road, 18));
        roads[18] = 0;
        check_roads();

        gs.roads_built += 1;
        ps[0].vertex = Vertex::ChooseInitialResources;
        ps[0].roads = 2;
        check_state();
        check_choose_initial_resources(0);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, { Edge::ChooseInitialResources, { { ArgType::NodeId, 26 } } });

        gs.is_second_round = false;
        gs.turn += 1;
        gs.round += 1;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].num_resources = 2;
        check_state();
        check_to_root(0);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check_state();
        check_roll_dice(0);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, { Edge::RollDice, { { ArgType::DiceRoll, 6 } } });

        gs.has_rolled = true;
        gs.dice_total = 6;
        ps[2].num_resources += 1;
        check_state();
        check_end_turn(0);
        check_no_actions(1);
        check_no_actions(2);

        // dump_actions();

        delete g;
    }
}

// NOLINTNEXTLINE(readability-function-size,google-readability-function-size)
TEST_CASE("Standard board scenarios", "[Game] [Game.Standard]")
{
    SECTION("Three players immediately ending turns")
    {
        auto b = Board::from_file("static/boards/Standard.board");
        auto s = get_standard_scenario();
        auto p = get_standard_parameters(3);
        auto g = Game::initialize(&b, s, p);

        bootstrap_tests();

        do_first_two_rounds_standard_3p();

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check_state();
        check_roll_dice(0);
        check_no_actions(1);
        check_no_actions(1);

        exec_ok(0, { Edge::RollDice, { { ArgType::DiceRoll, 10 } } });

        gs.has_rolled = true;
        gs.dice_total = 10;
        ps[0].vertex = Vertex::Root;
        ps[2].num_resources += 2;
        check_state();
        check_end_turn(0);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, { Edge::EndTurn, {} });

        gs.turn += 1;
        gs.has_rolled = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].is_current_player = false;
        ps[1].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_to_root(1);
        check_no_actions(2);

        exec_ok(1, { Edge::ToRoot, {} });

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_roll_dice(1);
        check_no_actions(2);

        exec_ok(1, { Edge::RollDice, { { ArgType::DiceRoll, 8 } } });

        gs.has_rolled = true;
        gs.dice_total = 8;
        ps[2].num_resources += 3;
        check_state();
        check_no_actions(0);
        check_end_turn(1);
        check_no_actions(2);

        exec_ok(1, { Edge::EndTurn, {} });

        gs.turn += 1;
        gs.has_rolled = false;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].is_current_player = false;
        ps[2].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_to_root(2);

        exec_ok(2, { Edge::ToRoot, {} });

        ps[2].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_roll_dice(2);

        exec_ok(2, { Edge::RollDice, { { ArgType::DiceRoll, 5 } } });

        gs.has_rolled = true;
        gs.dice_total = 5;
        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_end_turn(2);

        exec_ok(2, { Edge::EndTurn, {} });

        gs.turn += 1;
        gs.round += 1;
        gs.has_rolled = false;
        ps[0].is_current_player = true;
        ps[2].vertex = Vertex::WaitForTurn;
        ps[2].is_current_player = false;
        check_state();
        check_to_root(0);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, { Edge::ToRoot, {} });

        ps[0].vertex = Vertex::Root;
        check_state();
        check_roll_dice(0);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, { Edge::RollDice, { { ArgType::DiceRoll, 9 } } });

        gs.has_rolled = true;
        gs.dice_total = 9;
        check_state();
        check_end_turn(0);
        check_no_actions(1);
        check_no_actions(2);

        exec_ok(0, { Edge::EndTurn, {} });

        gs.turn += 1;
        gs.has_rolled = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[0].is_current_player = false;
        ps[1].is_current_player = true;
        check_state();
        check_no_actions(0);
        check_to_root(1);
        check_no_actions(2);

        exec_ok(1, { Edge::ToRoot, {} });

        ps[1].vertex = Vertex::Root;
        check_state();
        check_no_actions(0);
        check_roll_dice(1);
        check_no_actions(2);

        exec_ok(1, { Edge::RollDice, { { ArgType::DiceRoll, 4 } } });

        gs.has_rolled = true;
        gs.dice_total = 4;
        ps[0].num_resources += 1;
        check_state();
        check_no_actions(0);
        check_end_turn(1);
        check_no_actions(2);

        exec_ok(1, { Edge::EndTurn, {} });

        gs.turn += 1;
        gs.has_rolled = false;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[1].is_current_player = false;
        ps[2].is_current_player = true;

        exec_ok(2, { Edge::ToRoot, {} });

        ps[2].vertex = Vertex::Root;

        exec_ok(2, { Edge::RollDice, { { ArgType::DiceRoll, 10 } } });

        gs.has_rolled = true;
        gs.dice_total = 10;
        ps[2].num_resources += 2;
        check_state();

        // dump_actions();

        delete g;
    }

    SECTION("Three players offering trades")
    {
        auto b = Board::from_file("static/boards/Standard.board");
        auto s = get_standard_scenario();
        auto p = get_standard_parameters(3);
        auto g = Game::initialize(&b, s, p);

        bootstrap_tests();

        do_first_two_rounds_standard_3p();

        /*
        check_offer_trade(0, 1);
        exec_error(0,
                   trade({ 1 }, { { Resource::Brick, 2 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(0,
                   trade({ 1 }, { { Resource::Ore, 2 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(0,
                   trade({ 1 }, { { Resource::Sheep, 1 } }, { { Resource::Brick, 1 } }),
                   ResType::CannotAfford);
        exec_error(0,
                   trade({ 1 }, { { Resource::Wheat, 1 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(0,
                   trade({ 1 }, { { Resource::Wood, 1 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        // exec_error(0,trade({ 1}, { { Resource::Brick, 1 } }, { { Resource::Sheep, 1 } }),
        // ResType::CannotAfford); exec_error(0,trade({ 1}, { { Resource::Ore, 1 } }, { {
        // Resource::Sheep, 1 } }), ResType::CannotAfford);

        // ...

        check_offer_trade(1, 0);
        exec_error(1,
                   trade({ 0 }, { { Resource::Brick, 2 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(1,
                   trade({ 0 }, { { Resource::Ore, 1 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(1,
                   trade({ 0 }, { { Resource::Sheep, 1 } }, { { Resource::Brick, 1 } }),
                   ResType::CannotAfford);
        exec_error(1,
                   trade({ 0 }, { { Resource::Wheat, 1 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(1,
                   trade({ 0 }, { { Resource::Wood, 1 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        // exec_error(1,trade({ 0}, { { Resource::Brick, 1 } }, { { Resource::Sheep, 1 } }),
        // ResType::CannotAfford);

        // ...

        check_offer_trade(2, 0);
        exec_error(2,
                   trade({ 0 }, { { Resource::Brick, 1 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(2,
                   trade({ 0 }, { { Resource::Ore, 1 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(2,
                   trade({ 0 }, { { Resource::Sheep, 2 } }, { { Resource::Brick, 1 } }),
                   ResType::CannotAfford);
        exec_error(2,
                   trade({ 0 }, { { Resource::Wheat, 5 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        exec_error(2,
                   trade({ 0 }, { { Resource::Wood, 5 } }, { { Resource::Sheep, 1 } }),
                   ResType::CannotAfford);
        // exec_error(2,trade({ 0}, { { Resource::Sheep, 1 } }, { { Resource::Brick, 1 } }),
        // ResType::CannotAfford); exec_error(2,trade({ 0}, { { Resource::Wheat, 4 } }, { {
        // Resource::Sheep, 1 } }), ResType::CannotAfford); exec_error(2,trade({ 0}, { {
        // Resource::Wood, 4 } }, { { Resource::Sheep, 1 } }), ResType::CannotAfford);
        */

        exec_ok(0, { Edge::ToRoot, {} });
        exec_ok(0, { Edge::RollDice, { { ArgType::DiceRoll, 10 } } });
        exec_ok(0, { Edge::EndTurn, {} });

        exec_ok(1, { Edge::ToRoot, {} });
        exec_ok(1, { Edge::RollDice, { { ArgType::DiceRoll, 10 } } });
        exec_ok(1, { Edge::EndTurn, {} });

        exec_ok(2, { Edge::ToRoot, {} });
        exec_ok(2, { Edge::RollDice, { { ArgType::DiceRoll, 10 } } });

        gs.dice_total = 10;
        gs.has_rolled = true;
        gs.turn += 2;
        ps[0].is_current_player = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[2].is_current_player = true;
        ps[2].num_resources += 6;
        ps[2].vertex = Vertex::Root;

        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_end_turn(2);

        /*
         * p0 has { brick: 1, ore: 1 }
         * p1 has { brick: 1 }
         * p2 has { sheep: 1, wood: 4, wheat: 4 }
         */

        exec_ok(2, trade({ 0, 1 }, { { Resource::Wheat, 1 } }, { { Resource::Brick, 1 } }));

        gs.has_current_trade = true;
        gs.should_wait_for_trade = true;
        gs.num_trades_offered_this_turn += 1;
        ps[0].can_accept_trade = true;
        ps[1].can_accept_trade = true;
        ps[2].vertex = Vertex::WaitForTradeResponses;
        check_state();

        exec_ok(2, { Edge::CancelTrade, {} });

        gs.has_current_trade = false;
        gs.should_wait_for_trade = false;
        ps[0].can_accept_trade = false;
        ps[1].can_accept_trade = false;
        ps[2].vertex = Vertex::Root;
        check_state();

        exec_ok(2, trade({ 0, 1 }, { { Resource::Wheat, 1 } }, { { Resource::Brick, 1 } }));

        gs.has_current_trade = true;
        gs.should_wait_for_trade = true;
        gs.num_trades_offered_this_turn += 1;
        ps[0].can_accept_trade = true;
        ps[1].can_accept_trade = true;
        ps[2].vertex = Vertex::WaitForTradeResponses;

        exec_ok(0, { Edge::DeclineTrade, {} });

        ps[0].has_declined_trade = true;
        check_state();

        exec_ok(1, { Edge::DeclineTrade, {} });

        gs.should_wait_for_trade = false;
        ps[1].has_declined_trade = true;
        check_state();

        exec_ok(2, { Edge::FailTradeUnableToFindPartner, {} });

        gs.has_current_trade = false;
        ps[0].can_accept_trade = false;
        ps[0].has_declined_trade = false;
        ps[1].can_accept_trade = false;
        ps[1].has_declined_trade = false;
        ps[2].vertex = Vertex::Root;
        check_state();

        exec_ok(2, trade({ 0, 1 }, { { Resource::Wheat, 1 } }, { { Resource::Brick, 1 } }));

        gs.has_current_trade = true;
        gs.should_wait_for_trade = true;
        gs.num_trades_offered_this_turn += 1;
        ps[0].can_accept_trade = true;
        ps[1].can_accept_trade = true;
        ps[2].vertex = Vertex::WaitForTradeResponses;

        exec_ok(0, { Edge::AcceptTrade, {} });

        gs.has_current_trade = false;
        gs.should_wait_for_trade = false;
        ps[0].can_accept_trade = false;
        ps[0].has_declined_trade = false;
        ps[1].can_accept_trade = false;
        ps[1].has_declined_trade = false;
        ps[2].vertex = Vertex::Root;
        check_state();

        exec_ok(2, trade({ 0, 1 }, { { Resource::Wheat, 1 } }, { { Resource::Brick, 1 } }));
        exec_ok(1, { Edge::DeclineTrade, {} });
        exec_ok(2, { Edge::FailTradeUnableToFindPartner, {} });

        gs.num_trades_offered_this_turn += 1;
        check_state();

        exec_ok(2, trade({ 0, 1 }, { { Resource::Wheat, 1 } }, { { Resource::Wood, 1 } }));
        exec_ok(2, { Edge::FailTradeUnableToFindPartner, {} });

        gs.num_trades_offered_this_turn += 1;
        check_state();

        exec_ok(2, trade({ 0 }, { { Resource::Wheat, 1 } }, { { Resource::Brick, 1 } }));
        exec_ok(2, { Edge::FailTradeUnableToFindPartner, {} });

        gs.num_trades_offered_this_turn += 1;
        check_state();

        // dump_actions();

        delete g;
    }

    SECTION("Three players building roads")
    {
        auto b = Board::from_file("static/boards/Standard.board");
        auto s = get_standard_scenario();
        auto p = get_standard_parameters(3);
        auto g = Game::initialize(&b, s, p);

        bootstrap_tests();

        do_first_two_rounds_standard_3p();

        exec_ok(0, { Edge::ToRoot, {} });
        exec_ok(0, { Edge::RollDice, { { ArgType::DiceRoll, 10 } } });
        exec_ok(0, { Edge::EndTurn, {} });

        exec_ok(1, { Edge::ToRoot, {} });
        exec_ok(1, { Edge::RollDice, { { ArgType::DiceRoll, 10 } } });
        exec_ok(1, { Edge::EndTurn, {} });

        exec_ok(2, { Edge::ToRoot, {} });
        exec_ok(2, { Edge::RollDice, { { ArgType::DiceRoll, 10 } } });

        exec_ok(2, trade({ 0 }, { { Resource::Wheat, 1 } }, { { Resource::Brick, 1 } }));
        exec_ok(0, { Edge::AcceptTrade, {} });
        exec_ok(2, trade({ 1 }, { { Resource::Wheat, 1 } }, { { Resource::Brick, 1 } }));
        exec_ok(1, { Edge::AcceptTrade, {} });

        gs.dice_total = 10;
        gs.has_rolled = true;
        gs.turn += 2;
        gs.num_trades_offered_this_turn = 2;
        ps[0].is_current_player = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[2].is_current_player = true;
        ps[2].num_resources += 6;
        ps[2].vertex = Vertex::Root;

        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_end_turn(2);

        /*
         * p0 has { ore: 1, wheat: 1 }
         * p1 has { wheat: 1 }
         * p2 has { brick: 2, sheep: 1, wood: 4, wheat: 2 }
         */

        check_build_road(2, 8);

        exec_ok(2, build(Building::Road, 48));
        roads[48] = 2;
        check_roads();

        gs.roads_built += 1;
        ps[2].roads += 1;
        ps[2].num_resources -= 2;
        check_state();

        check_build_road(2, 9);

        exec_error(2, build(Building::Road, 32), ResType::InvalidNodeId);

        exec_ok(2, build(Building::Road, 33));
        roads[33] = 2;
        check_roads();

        gs.roads_built += 1;
        ps[2].roads += 1;
        ps[2].num_resources -= 2;
        check_state();

        // dump_actions();

        delete g;
    }

    SECTION("Three players building settlements")
    {
        auto b = Board::from_file("static/boards/Standard.board");
        auto s = get_standard_scenario();
        auto p = get_standard_parameters(3);
        auto g = Game::initialize(&b, s, p);

        bootstrap_tests();

        do_first_two_rounds_standard_3p();

        exec_ok(0, { Edge::ToRoot, {} });
        exec_ok(0, { Edge::RollDice, { { ArgType::DiceRoll, 10 } } });
        exec_ok(0, { Edge::EndTurn, {} });

        exec_ok(1, { Edge::ToRoot, {} });
        exec_ok(1, { Edge::RollDice, { { ArgType::DiceRoll, 10 } } });
        exec_ok(1, { Edge::EndTurn, {} });

        exec_ok(2, { Edge::ToRoot, {} });
        exec_ok(2, { Edge::RollDice, { { ArgType::DiceRoll, 10 } } });

        exec_ok(2, trade({ 0 }, { { Resource::Wheat, 1 } }, { { Resource::Brick, 1 } }));
        exec_ok(0, { Edge::AcceptTrade, {} });

        gs.dice_total = 10;
        gs.has_rolled = true;
        gs.turn += 2;
        gs.num_trades_offered_this_turn = 1;
        ps[0].is_current_player = false;
        ps[0].vertex = Vertex::WaitForTurn;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[2].is_current_player = true;
        ps[2].num_resources += 6;
        ps[2].vertex = Vertex::Root;

        check_state();
        check_no_actions(0);
        check_no_actions(1);
        check_end_turn(2);

        /*
         * p0 has { ore: 1, wheat: 1 }
         * p1 has { brick: 1 }
         * p2 has { brick: 1, sheep: 1, wood: 4, wheat: 3 }
         */

        check_build_settlement(2, 1);

        exec_ok(2, build(Building::Settlement, 56));
        settlements[56] = 2;
        settlements[40] = -1;
        check_settlements();

        gs.settlements_built += 1;
        ps[2].settlements += 1;
        ps[2].num_resources -= 4;
        ps[2].public_victory_points += 1;
        check_state();

        // dump_actions();

        delete g;
    }

    SECTION("Three players buying development cards")
    {
        auto b = Board::from_file("static/boards/Standard.board");
        auto s = get_standard_scenario();
        auto p = get_standard_parameters(3);
        auto g = Game::initialize(&b, s, p);

        bootstrap_tests();

        do_first_two_rounds_standard_3p();

        const size_t rounds = 25;

        for (size_t i = 0; i < rounds; ++i) {
            exec_ok(0, { Edge::ToRoot, {} });
            exec_ok(0, { Edge::RollDice, { { ArgType::DiceRoll, 4 } } });
            exec_ok(0, { Edge::EndTurn, {} });

            exec_ok(1, { Edge::ToRoot, {} });
            exec_ok(1, { Edge::RollDice, { { ArgType::DiceRoll, 8 } } });
            exec_ok(1, { Edge::EndTurn, {} });

            exec_ok(2, { Edge::ToRoot, {} });
            exec_ok(2, { Edge::RollDice, { { ArgType::DiceRoll, 8 } } });
            exec_ok(2, { Edge::EndTurn, {} });
        }

        exec_ok(0, { Edge::ToRoot, {} });
        exec_ok(0, { Edge::RollDice, { { ArgType::DiceRoll, 4 } } });

        /*
         * p0 has { brick: 1, ore: 2 + rounds }
         * p1 has { brick: 1 }
         * p2 has { sheep: 1 + 2 * rounds, wheat: 1 + rounds, wood: 1 }
         */

        exec_ok(
            0,
            trade({ 2 },
                  { { Resource::Brick, 1 } },
                  { { Resource::Sheep, 1 + 4 * rounds }, { Resource::Wheat, 1 + 2 * rounds } }));
        exec_ok(2, { Edge::AcceptTrade, {} });

        gs.dice_total = 4;
        gs.has_rolled = true;
        gs.turn += rounds * 3;
        gs.round += rounds;
        gs.num_trades_offered_this_turn = 1;
        ps[0].vertex = Vertex::Root;
        ps[0].num_resources += 2 + rounds * 7;
        ps[1].vertex = Vertex::WaitForTurn;
        ps[2].vertex = Vertex::WaitForTurn;
        ps[2].num_resources = 2;

        check_state();
        check_end_turn(0);
        check_no_actions(1);
        check_no_actions(2);

        for (size_t i = 0; i < rounds; ++i) {
            exec_ok(
                0,
                { Edge::Build,
                  { { ArgType::BuildItemId, static_cast<size_t>(Building::DevelopmentCard) } } });
        }

        // no more development cards
        exec_error(0,
                   { Edge::Build,
                     { { ArgType::BuildItemId, static_cast<size_t>(Building::DevelopmentCard) } } },
                   ResType::InvalidEdgeChoice);

        gs.development_cards_built += rounds;
        ps[0].num_resources -= rounds * 3;
        ps[0].num_unplayed_development_cards += rounds;
        check_state();

        exec_ok(0, { Edge::EndTurn, {} });

        exec_ok(1, { Edge::ToRoot, {} });
        exec_ok(1, { Edge::RollDice, { { ArgType::DiceRoll, 12 } } });
        exec_ok(1, { Edge::EndTurn, {} });

        exec_ok(2, { Edge::ToRoot, {} });
        exec_ok(2, { Edge::RollDice, { { ArgType::DiceRoll, 12 } } });
        exec_ok(2, { Edge::EndTurn, {} });

        exec_ok(0, { Edge::ToRoot, {} });
        exec_ok(0, { Edge::RollDice, { { ArgType::DiceRoll, 12 } } });

        gs.dice_total = 12;
        gs.turn += 3;
        gs.round += 1;
        gs.num_trades_offered_this_turn = 0;
        check_state();

        exec_error(0,
                   { Edge::PlayDevelopmentCard,
                     { { ArgType::DevelopmentCardId, static_cast<size_t>(DevelopmentCard::Knight) },
                       { ArgType::NodeId, 143 } } },
                   ResType::InvalidNodeId);

        exec_ok(0,
                { Edge::PlayDevelopmentCard,
                  { { ArgType::DevelopmentCardId, static_cast<size_t>(DevelopmentCard::Knight) },
                    { ArgType::NodeId, 141 } } });

        gs.robber_location = 141;
        ps[0].num_played_development_cards += 1;
        ps[0].num_unplayed_development_cards -= 1;
        ps[0].vertex = Vertex::AfterMovingRobber;
        ps[0].army_size += 1;
        check_state();

        exec_ok(0, { Edge::ToRoot, {} });

        exec_ok(0,
                { Edge::PlayDevelopmentCard,
                  { { ArgType::DevelopmentCardId, static_cast<size_t>(DevelopmentCard::Knight) },
                    { ArgType::NodeId, 19 } } });

        exec_ok(0, { Edge::ToRoot, {} });

        exec_ok(0,
                { Edge::PlayDevelopmentCard,
                  { { ArgType::DevelopmentCardId, static_cast<size_t>(DevelopmentCard::Knight) },
                    { ArgType::NodeId, 21 } } });

        gs.robber_location = 21;
        gs.has_largest_army = 0;
        gs.largest_army = 3;
        gs.can_steal = true;
        ps[0].num_played_development_cards += 2;
        ps[0].num_unplayed_development_cards -= 2;
        ps[0].army_size += 2;
        ps[0].public_victory_points += 2;
        check_state();

        exec_error(0, { Edge::Steal, { { ArgType::PlayerId, 0 } } }, ResType::InvalidPlayerId);
        exec_error(0, { Edge::Steal, { { ArgType::PlayerId, 2 } } }, ResType::InvalidPlayerId);
        exec_ok(0, { Edge::Steal, { { ArgType::PlayerId, 1 } } });

        gs.can_steal = false;
        ps[0].num_resources += 1;
        ps[1].num_resources -= 1;
        check_state();

        exec_ok(0, { Edge::ToRoot, {} });
        exec_ok(0,
                { Edge::PlayDevelopmentCard,
                  { { ArgType::DevelopmentCardId, static_cast<size_t>(DevelopmentCard::Monopoly) },
                    { ArgType::TakeResourceType, static_cast<size_t>(Resource::Ore) } } });
        exec_ok(0,
                { Edge::PlayDevelopmentCard,
                  { { ArgType::DevelopmentCardId, static_cast<size_t>(DevelopmentCard::Monopoly) },
                    { ArgType::TakeResourceType, static_cast<size_t>(Resource::Wood) } } });

        ps[0].vertex = State::Vertex::Root;
        ps[0].num_played_development_cards += 2;
        ps[0].num_unplayed_development_cards -= 2;
        ps[0].num_resources += 1;
        ps[2].num_resources -= 1;
        check_state();

        exec_error(
            0,
            { Edge::PlayDevelopmentCard,
              { { ArgType::DevelopmentCardId, static_cast<size_t>(DevelopmentCard::RoadBuilding) },
                { ArgType::NodeId, 8 },
                { ArgType::NodeId, 22 } } },
            ResType::InvalidNodeId);

        exec_ok(
            0,
            { Edge::PlayDevelopmentCard,
              { { ArgType::DevelopmentCardId, static_cast<size_t>(DevelopmentCard::RoadBuilding) },
                { ArgType::NodeId, 8 },
                { ArgType::NodeId, 20 } } });

        roads[8] = 0;
        roads[20] = 0;
        check_roads();

        gs.roads_built += 2;
        ps[0].roads += 2;
        ps[0].num_played_development_cards += 1;
        ps[0].num_unplayed_development_cards -= 1;
        check_state();

        for (size_t i = 0; i < 5; ++i) {
            exec_ok(0,
                    { Edge::PlayDevelopmentCard,
                      { { ArgType::DevelopmentCardId,
                          static_cast<size_t>(DevelopmentCard::VictoryPoint) } } });
        }

        ps[0].public_victory_points += 5;
        ps[0].num_played_development_cards += 5;
        ps[0].num_unplayed_development_cards -= 5;
        check_state();

        exec_ok(
            0,
            { Edge::PlayDevelopmentCard,
              { { ArgType::DevelopmentCardId, static_cast<size_t>(DevelopmentCard::YearOfPlenty) },
                { ArgType::TakeResourceType, static_cast<size_t>(Resource::Wood) },
                { ArgType::TakeResourceType, static_cast<size_t>(Resource::Wood) } } });

        ps[0].num_resources += 1;
        ps[0].num_played_development_cards += 1;
        ps[0].num_unplayed_development_cards -= 1;
        check_state();

        dump_actions();

        delete g;
    }
}
