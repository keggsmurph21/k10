#include <cassert>
#include <iostream>
#include <stdexcept>
#include <variant>

#include "Board/Store.h"
#include "Game/Game.h"
#include "Scenario/Store.h"

namespace k10engine::Game {

size_t Game::get_dice_total() const
{
    return m_dice.get_total();
}

bool Game::is_first_round() const
{
    return get_round() == 0;
}

bool Game::is_second_round() const
{
    return get_round() == 1;
}

bool Game::is_roll_seven() const
{
    return get_dice_total() == k10_ROBBER_DICE_ROLL;
}

bool Game::should_wait_for_discard() const
{
    for (const auto& player : m_players) {
        if (player->num_to_discard() > 0) {
            return true;
        }
    }
    return false;
}

bool Game::should_wait_for_trade() const
{
    for (const auto& player : m_players) {
        if (player->can_accept_trade() && !player->has_declined_trade()) {
            return true;
        }
    }
    return false;
}

void Game::set_current_trade(std::optional<Trade> trade)
{
    m_current_trade = trade;
    if (trade.has_value()) {
        for (auto& player : trade->offered_to) {
            if (player->can_afford(trade->to_offerer)) {
                player->set_can_accept_trade(true);
                player->set_has_declined_trade(false);
            }
        }
        ++m_num_trades_offered_this_turn;
    } else {
        for (auto& player : m_players) {
            player->set_can_accept_trade(false);
            player->set_has_declined_trade(false);
        }
    }
}

size_t Game::get_round() const
{
    return m_turn / m_players.size();
}

Game::~Game()
{
    for (auto player : m_players)
        delete player;
    for (auto node : m_nodes)
        delete node;
    m_deck.clear();
}

Game* Game::initialize(PlayerId owner, const Scenario::Scenario* scenario, const Scenario::Parameters& parameters)
{
    if (!scenario->is_valid(parameters)) {
        return {};
    }

    const auto& deck = scenario->get_development_card_deck(parameters.development_card_iteration_type);
    const auto& ports = scenario->get_ports(parameters.port_iteration_type);
    const auto& resources = scenario->get_resources(parameters.resource_iteration_type);
    const auto& rolls = scenario->get_rolls(parameters.roll_iteration_type);

    std::vector<BoardView::NodeView*> nodes;
    std::map<size_t, BoardView::Junction*> junctions;
    std::map<size_t, BoardView::Road*> roads;

    int robber_index = -1;

    size_t resource_index = 0;
    size_t roll_index = 0;

    auto make_hex = [&](const Board::Node& node) -> BoardView::Hex* {
        if (resource_index >= resources.size()) {
            return nullptr; // Too few resources
        }
        const auto& resource = resources.at(resource_index);
        if (std::holds_alternative<NonYieldingResource>(resource)) {
            if (robber_index != -1) {
                return nullptr; // FIXME: Handle multiple robbers?
            }
            robber_index = static_cast<int>(node.index());
            return new BoardView::Hex(node, resource, 0);
        }
        if (roll_index >= rolls.size()) {
            return nullptr; // Too few rolls
        }
        const auto roll = rolls.at(roll_index);
        ++roll_index;
        return new BoardView::Hex(node, resource, roll);
    };

    auto make_junction = [&](const Board::Node& node) -> BoardView::Junction* {
        const auto port = scenario->graph().port(node);
        if (port == nullptr) {
            return new BoardView::Junction(node, {}, 0);
        }
        const auto port_index = port->index();
        if (port_index >= ports.size()) {
            return nullptr; // Too few ports
        }
        const auto& port_spec = ports.at(port_index);
        return new BoardView::Junction(node, port_spec.resources, port_spec.exchange_rate);
    };

    auto make_road = [&](const Board::Node& node) -> BoardView::Road* { return new BoardView::Road(node); };

    for (const auto& node : scenario->graph().nodes()) {
        switch (node.type()) {
        case Board::NodeType::Hex: { // scope for const
            auto hex = make_hex(node);
            nodes.push_back(hex);
            ++resource_index;
        } break;
        case Board::NodeType::Junction: { // scope for const
            auto junction = make_junction(node);
            nodes.push_back(junction);
            junctions[node.index()] = junction;
        } break;
        case Board::NodeType::Road: { // scope for const
            auto road = make_road(node);
            nodes.push_back(road);
            roads[node.index()] = road;
        } break;
        case Board::NodeType::Ocean:
        case Board::NodeType::UnflippedHex:
            nodes.push_back(nullptr);
            break; // do nothing
        }
    }

    for (auto node : nodes) {
        if (node == nullptr) {
            continue;
        }
        for (const auto& direction : Board::AllDirections) {
            const auto neighbor_node = scenario->graph().neighbor(node->node(), direction);
            if (neighbor_node == nullptr) {
                continue;
            }
            auto* neighbor = nodes.at(neighbor_node->index());
            if (neighbor == nullptr) {
                continue;
            }
            if ((node->is_hex() && neighbor->is_junction()) || (node->is_junction() && neighbor->is_hex())
                || (node->is_junction() && neighbor->is_road()) || (node->is_road() && neighbor->is_junction())) {
                node->add_neighbor_(direction, neighbor);
            }
        }
    }

    if (robber_index < 0 || nodes.size() <= static_cast<size_t>(robber_index)) {
        return {}; // something went wrong ... or no Desert given
    }

    // FIXME: Don't just blindly cast this unless we're 100% sure this is a Hex ...
    auto robber_location = static_cast<BoardView::Hex*>(nodes.at(robber_index));

    auto* game = new Game(owner, scenario, nodes, deck, parameters.victory_points_goal, robber_location);
    for (size_t i = 0; i < parameters.players_count; ++i) {
        game->m_players.push_back(new Player(0, i, game));
    }
    game->current_player().set_vertex(State::Vertex::Root);
    return game;
}

static bool player_can_execute_edge(const Player& player, const State::Edge& requested_edge)
{
    const auto valid_actions = player.get_available_actions();
    for (const auto& valid_action : valid_actions) {
        if (valid_action.edge == requested_edge) {
            return true;
        }
    }
    return false;
}

static bool is_road_reachable_for(const BoardView::Road* road, const Player& player)
{
    if (road->owner() != nullptr) {
        return false;
    }
    bool is_reachable = false;
    road->for_each_junction_neighbor([&](auto, const auto* junction) {
        if (junction->owner() != nullptr) {
            if (*junction->owner() == player) {
                is_reachable = true;
                return;
            }
            return;
        }
        junction->for_each_road_neighbor([&](auto, const auto* other_road) {
            if (other_road == road) {
                return;
            }
            if (other_road->owner() != nullptr && *other_road->owner() == player) {
                is_reachable = true;
                return;
            }
        });
    });
    return is_reachable;
}

Result Game::execute_accept_trade(Player& player)
{
    if (!player_can_execute_edge(player, State::Edge::AcceptTrade)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    // std::cout << "before:" << std::endl;
    // std::cout << "  " << current_trade()->offerer->m_resources << std::endl;
    // std::cout << "  " << player.m_resources << std::endl;

    current_trade()->offerer->accrue_resources(current_trade()->to_offerer);
    current_trade()->offerer->spend_resources(current_trade()->from_offerer);

    player.accrue_resources(current_trade()->from_offerer);
    player.spend_resources(current_trade()->to_offerer);

    // std::cout << "after:" << std::endl;
    // std::cout << "  " << current_trade()->offerer->m_resources << std::endl;
    // std::cout << "  " << player.m_resources << std::endl;

    set_current_trade({});
    current_trade()->offerer->set_vertex(State::Vertex::Root);
    return { ResultType::Ok, {} };
}

Result Game::execute_build_city(Player& player, BoardView::Junction* junction)
{
    if (!player_can_execute_edge(player, State::Edge::BuildCity)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    if (*junction->owner() != player) {
        return { ResultType::InvalidNodeId, {} };
    }

    build_city(player, junction, Options::None);

    if (is_game_over()) {
        assert(false);
    }

    player.set_vertex(State::Vertex::Root);
    return { ResultType::Ok, {} };
}

Result Game::execute_build_development_card(Player& player)
{
    if (!player_can_execute_edge(player, State::Edge::BuildDevelopmentCard)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    if (!player.can_afford(Building::DevelopmentCard)) {
        return { ResultType::CannotAfford, {} };
    }

    const auto development_card = build_development_card(player, Options::None);

    if (is_game_over()) {
        assert(false);
    } else {
        player.set_vertex(State::Vertex::Root);
    }

    return { ResultType::Ok, { { ActionArgumentType::DevelopmentCardId, static_cast<size_t>(development_card) } } };
}

Result Game::execute_build_road(Player& player, BoardView::Road* road)
{
    if (!player_can_execute_edge(player, State::Edge::BuildRoad)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    if (!is_road_reachable_for(road, player)) {
        return { ResultType::InvalidNodeId, {} };
    }

    if (is_first_round() || is_second_round()) {
        build_road(player, road, Options::NoCost);
    } else if (player.can_afford(Building::Road)) {
        build_road(player, road, Options::None);
    } else {
        return { ResultType::CannotAfford, {} };
    }

    if (is_game_over()) {
        assert(false);
    } else if (is_first_round()) {
        player.set_vertex(State::Vertex::WaitForTurn);
        increment_turn();
    } else if (is_second_round()) {
        player.set_vertex(State::Vertex::ChooseInitialResources);
    } else {
        player.set_vertex(State::Vertex::Root);
    }

    return { ResultType::Ok, {} };
}

Result Game::execute_build_settlement(Player& player, BoardView::Junction* junction)
{
    if (!player_can_execute_edge(player, State::Edge::BuildSettlement)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    if (!junction->is_settleable()) {
        return { ResultType::JunctionNotSettleable, {} };
    }

    if (is_first_round() || is_second_round()) {
        build_settlement(player, junction, Options::NoCost);
    } else if (player.can_afford(Building::Settlement)) {
        build_settlement(player, junction, Options::None);
    } else {
        return { ResultType::CannotAfford, {} };
    }

    if (is_game_over()) {
        assert(false);
    } else if (is_first_round() || is_second_round()) {
        player.set_vertex(State::Vertex::AfterBuildingFreeSettlement);
    } else {
        player.set_vertex(State::Vertex::Root);
    }

    return { ResultType::Ok, {} };
}

Result Game::execute_cancel_trade(Player& player)
{
    if (!player_can_execute_edge(player, State::Edge::CancelTrade)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    set_current_trade({});
    player.set_vertex(State::Vertex::Root);
    return { ResultType::Ok, {} };
}

Result Game::execute_choose_initial_resources(Player& player, const BoardView::Junction* junction)
{
    if (!player_can_execute_edge(player, State::Edge::ChooseInitialResources)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    for (const auto& settlement : player.settlements()) {
        if (settlement == junction) {
            settlement->for_each_hex_neighbor([&](auto, const auto* hex) {
                const auto& hex_resource = hex->resource();
                if (std::holds_alternative<Resource>(hex_resource)) {
                    player.accrue_resources({ { std::get<Resource>(hex_resource), 1 } });
                }
            });
            player.set_vertex(State::Vertex::WaitForTurn);
            increment_turn();
            return { ResultType::Ok, {} };
        }
    }
    return { ResultType::InvalidNodeId, {} };
}

Result Game::execute_decline_trade(Player& player)
{
    if (!player_can_execute_edge(player, State::Edge::DeclineTrade)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    player.set_has_declined_trade(true);
    if (!should_wait_for_trade()) {
        set_current_trade({});
        current_player().set_vertex(State::Vertex::Root);
    }
    return { ResultType::Ok, {} };
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
Result Game::execute_discard(Player& player, const ResourceCounts& resources)
{
    if (!player_can_execute_edge(player, State::Edge::Discard)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    if (resources.empty()) {
        return { ResultType::InvalidTrade, {} };
    }
    if (!player.can_afford(resources)) {
        return { ResultType::CannotAfford, {} };
    }

    size_t num_to_discard = player.num_to_discard();
    size_t num_offered = 0;
    for (const auto& it : resources) {
        const auto& count = it.second;
        num_offered += count;
    }

    if (num_offered > num_to_discard) {
        return { ResultType::StopFlexing, {} };
    }

    player.spend_resources(resources);
    player.set_num_to_discard(num_to_discard - num_offered);

    return { ResultType::Ok, {} };
}

Result Game::execute_end_turn(Player& player)
{
    if (!player_can_execute_edge(player, State::Edge::EndTurn)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    player.set_vertex(State::Vertex::WaitForTurn);
    increment_turn();
    return { ResultType::Ok, {} };
}

Result Game::execute_move_robber(Player& player, const BoardView::Hex* hex)
{
    if (!player_can_execute_edge(player, State::Edge::MoveRobber)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    if (hex->index() == robber_location()->index()) {
        return { ResultType::InvalidNodeId, {} };
    }

    move_robber(player, hex);
    player.set_vertex(State::Vertex::AfterMovingRobber);

    return { ResultType::Ok, {} };
}

Result Game::execute_offer_trade(Player& player, const Trade& trade)
{
    if (!player_can_execute_edge(player, State::Edge::OfferTrade)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    if (trade.offered_to.empty()) {
        return { ResultType::InvalidTrade, {} };
    }
    if (trade.from_offerer.empty()) {
        return { ResultType::InvalidTrade, {} };
    }
    if (trade.to_offerer.empty()) {
        return { ResultType::InvalidTrade, {} };
    }
    if (trade.from_offerer == trade.to_offerer) {
        return { ResultType::InvalidTrade, {} };
    }
    if (!player.can_afford(trade.from_offerer)) {
        return { ResultType::CannotAfford, {} };
    }

    set_current_trade(trade);
    player.set_vertex(State::Vertex::WaitForTradeResponses);

    if (!should_wait_for_trade()) {
        set_current_trade({});
        current_player().set_vertex(State::Vertex::Root);
    }

    return { ResultType::Ok, {} };
}

void Game::move_robber(const Player& player, const BoardView::Hex* hex)
{
    m_robber.set_location(hex);
    hex->for_each_junction_neighbor([&](auto, const auto* junction) {
        if (junction->owner() == nullptr) {
            return;
        }
        if (*junction->owner() == player) {
            return;
        }
        set_can_steal(true);
    });
}

Result Game::execute_play_knight(Player& player, const BoardView::Hex* hex)
{
    if (!player_can_execute_edge(player, State::Edge::PlayKnight)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    if (hex->index() == robber_location()->index()) {
        return { ResultType::InvalidNodeId, {} };
    }

    move_robber(player, hex);
    player.play_knight(hex);
    player.set_vertex(State::Vertex::AfterMovingRobber);
    recalculate_largest_army(&player);

    return { ResultType::Ok, {} };
}

Result Game::execute_play_monopoly(Player& player, const Resource& resource)
{
    if (!player_can_execute_edge(player, State::Edge::PlayMonopoly)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    for (auto& other_player : m_players) {
        if (*other_player == player) {
            continue;
        }
        const auto count = other_player->count(resource);
        other_player->spend_resources({ { resource, count } });
        player.accrue_resources({ { resource, count } });
    }

    player.play_monopoly(resource);
    player.set_vertex(State::Vertex::Root);

    return { ResultType::Ok, {} };
}

Result Game::execute_play_road_building(Player& player, BoardView::Road* road_0, BoardView::Road* road_1)
{
    if (!player_can_execute_edge(player, State::Edge::PlayRoadBuilding)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    if (!is_road_reachable_for(road_0, player)) {
        return { ResultType::InvalidNodeId, {} };
    }

    if (!is_road_reachable_for(road_1, player)) {
        // This road isn't reachable yet, but it might be reachable
        // once the first road is actually built.
        bool is_road_1_adjacent_to_road_0 = false;
        road_0->for_each_junction_neighbor([&](auto, const auto* junction) {
            if (is_road_1_adjacent_to_road_0) {
                return;
            }
            const auto& owner = junction->owner();
            if (owner != nullptr && *owner != player) {
                return;
            }
            junction->for_each_road_neighbor([&](auto, const auto* road) {
                if (road == road_1) {
                    is_road_1_adjacent_to_road_0 = true;
                    return;
                }
            });
        });
        if (!is_road_1_adjacent_to_road_0) {
            return { ResultType::InvalidNodeId, {} };
        }
    }

    build_road(player, road_0, Options::NoCost);
    build_road(player, road_1, Options::NoCost);

    player.play_road_building(road_0, road_0);
    player.set_vertex(State::Vertex::Root);

    return { ResultType::Ok, {} };
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
Result Game::execute_play_victory_point(Player& player)
{
    if (!player_can_execute_edge(player, State::Edge::PlayVictoryPoint)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    player.play_victory_point();
    player.set_vertex(State::Vertex::Root);

    return { ResultType::Ok, {} };
}
// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
Result Game::execute_play_year_of_plenty(Player& player, const Resource& resource_0, const Resource& resource_1)
{
    if (!player_can_execute_edge(player, State::Edge::PlayYearOfPlenty)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    player.accrue_resources({ { resource_0, 1 }, { resource_1, 1 } });
    player.play_year_of_plenty(resource_0, resource_1);
    player.set_vertex(State::Vertex::Root);

    return { ResultType::Ok, {} };
}

Result Game::execute_roll_dice(Player& player)
{
    if (!player_can_execute_edge(player, State::Edge::RollDice)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    m_dice.roll();
    while (is_first_round() && is_roll_seven()) {
        m_dice.roll();
    }
    return _after_roll(player);
}

#ifdef k10_ENABLE_ROLL_DICE_EXACT
Result Game::execute_roll_dice(Player& player, size_t roll)
{
    if (!player_can_execute_edge(player, State::Edge::RollDice)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    if (roll < 2 || 12 < roll) {
        return { ResultType::DiceRollOutOfRange, {} };
    }
    m_dice.set_total(roll);
    return _after_roll(player);
}
#endif

Result Game::_after_roll(Player& player)
{
    m_has_rolled = true;

    const auto dice_total = get_dice_total();

    if (is_roll_seven()) {
        for (auto& other_player : m_players) {
            if (other_player->has_heavy_purse()) {
                size_t num_to_discard = other_player->num_resources() / 2;
                other_player->set_num_to_discard(num_to_discard);
            }
        }
        player.set_vertex(State::Vertex::AfterRollingSeven);
    } else {
        for_each_hex([&](const auto* hex) {
            if (hex->roll_number() != dice_total) {
                return;
            }
            const auto& hex_resource = hex->resource();
            if (std::holds_alternative<NonYieldingResource>(hex_resource)) {
                return;
            }
            const auto& resource = std::get<Resource>(hex_resource);
            hex->for_each_junction_neighbor([&](auto, const auto* junction) {
                if (junction->owner() == nullptr) {
                    return;
                }
                const auto count = junction->has_city() ? 2 : 1;
                m_players.at(junction->owner()->index())->accrue_resources({ { resource, count } });
            });
        });
    }

    return { ResultType::Ok, { { ActionArgumentType::DiceRoll, dice_total } } };
}

Result Game::execute_steal(Player& player, Player& steal_from)
{
    if (!player_can_execute_edge(player, State::Edge::Steal)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    if (steal_from == player) {
        return { ResultType::InvalidPlayerId, {} };
    }

    if (steal_from.num_resources() == 0) {
        return { ResultType::InvalidPlayerId, {} };
    }

    bool is_adjacent = false;
    robber_location()->for_each_junction_neighbor([&](auto, const auto* junction) {
        const auto junction_owner = junction->owner();
        if (junction_owner != nullptr && *junction_owner == steal_from) {
            is_adjacent = true;
            return;
        }
    });
    if (!is_adjacent) {
        return { ResultType::InvalidPlayerId, {} };
    }

    const auto stolen_resource = choose_from(steal_from.m_resources);
    steal_from.spend_resources({ { stolen_resource, 1 } });
    player.accrue_resources({ { stolen_resource, 1 } });

    set_can_steal(false);

    return { ResultType::Ok, { { ActionArgumentType::TakeResourceType, static_cast<size_t>(stolen_resource) } } };
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
Result Game::execute_to_root(Player& player)
{
    if (!player_can_execute_edge(player, State::Edge::ToRoot)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    player.set_vertex(State::Vertex::Root);
    return { ResultType::Ok, {} };
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
Result Game::execute_trade_with_bank(Player& player, const Trade& trade)
{
    if (!player_can_execute_edge(player, State::Edge::TradeWithBank)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    // Ignore trade->offered_to
    if (trade.from_offerer.empty()) {
        return { ResultType::InvalidTrade, {} };
    }
    if (trade.to_offerer.empty()) {
        return { ResultType::InvalidTrade, {} };
    }
    if (!player.can_afford(trade.from_offerer)) {
        return { ResultType::CannotAfford, {} };
    }

    size_t resources_requested = 0;
    for (const auto& to : trade.to_offerer) {
        const auto& count_to = to.second;
        resources_requested += count_to;
    }

    size_t resources_offered = 0;
    for (const auto& from : trade.from_offerer) {
        const auto& resource_from = from.first;
        const auto& count_from = from.second;
        const auto& rate = player.bank_trade_rate(resource_from);
        const auto& count_to = count_from / rate; // floor division
        if (count_from > count_to * rate) {
            return { ResultType::InvalidTrade, {} };
        }
        resources_offered += count_to;
    }

    if (resources_offered > resources_requested) {
        return { ResultType::StopFlexing, {} };
    }
    if (resources_offered < resources_requested) {
        return { ResultType::InvalidTrade, {} };
    }

    player.accrue_resources(trade.to_offerer);
    player.spend_resources(trade.from_offerer);

    return { ResultType::Ok, {} };
}

void Game::increment_num_built(Building building)
{
    if (m_buildings_built.find(building) == m_buildings_built.end()) {
        m_buildings_built[building] = 0;
    }
    m_buildings_built[building] += 1;
}

DevelopmentCard Game::draw_development_card()
{
    const auto development_card = m_deck.at(m_deck_index);
    ++m_deck_index;
    return development_card;
}

void Game::build_city(Player& player, BoardView::Junction* junction, Options options)
{
    player.build_city(junction, options);
    increment_num_built(Building::City);
}

DevelopmentCard Game::build_development_card(Player& player, Options options)
{
    const auto development_card = draw_development_card();
    player.build_development_card(development_card, options);
    increment_num_built(Building::DevelopmentCard);
    // FIXME: Check if we drew a VictoryPoint
    return development_card;
}

void Game::build_settlement(Player& player, BoardView::Junction* junction, Options options)
{
    player.build_settlement(junction, options);
    increment_num_built(Building::Settlement);
}

void Game::build_road(Player& player, BoardView::Road* road, Options options)
{
    player.build_road(road, options);
    recalculate_longest_road();
    increment_num_built(Building::Road);
}

bool Game::is_game_over() const
{
    for (const auto& player : m_players) {
        if (player->private_victory_points() == m_victory_points_goal) {
            return true;
        }
    }
    return false;
}

void Game::increment_turn()
{
    ++m_turn;
    if (m_turn == m_players.size()) { // NOLINT(bugprone-branch-clone)
        // The first round is now over.  Keep m_current_player_index the same.
    } else if (m_turn == m_players.size() * 2) { // NOLINT(bugprone-branch-clone)
        // The second round is now over.  Keep m_current_player_index the same.
    } else if (is_second_round()) {
        // It's the second round, so we iterate backwards.
        --m_current_player_index;
    } else {
        // The default case.
        ++m_current_player_index;
        if (m_current_player_index == m_players.size()) {
            m_current_player_index = 0;
        }
    }
    m_has_rolled = false;
    m_num_trades_offered_this_turn = 0;
    for (auto& player : m_players) {
        for (auto& development_card : player->m_unplayable_development_cards) {
            player->m_playable_development_cards.push_back(development_card);
        }
        player->m_unplayable_development_cards.clear();
    }
}

size_t Game::largest_army() const
{
    if (m_has_largest_army == nullptr) {
        return k10_LARGEST_ARMY_THRESHOLD;
    }
    return m_has_largest_army->army_size();
}

size_t Game::longest_road() const
{
    if (m_has_longest_road == nullptr) {
        return k10_LONGEST_ROAD_THRESHOLD;
    }
    return m_has_longest_road->longest_road();
}

size_t Game::num_built(Building building) const
{
    if (m_buildings_built.find(building) == m_buildings_built.end()) {
        return 0;
    }
    return m_buildings_built.at(building);
}

void Game::recalculate_longest_road()
{
    // FIXME: Implement me plz :^)
}

void Game::recalculate_largest_army(Player* player)
{
    if (m_has_largest_army == player) {
        return;
    }
    if (player->army_size() > largest_army()) {
        if (m_has_largest_army != nullptr) {
            m_has_largest_army->m_private_victory_points -= 2;
            m_has_largest_army->m_public_victory_points -= 2;
        }
        player->m_private_victory_points += 2;
        player->m_public_victory_points += 2;
        m_has_largest_army = player;
    }
}

std::ostream& operator<<(std::ostream& os, const Game& game)
{
    const auto robber_hex = game.robber_location();
    for (size_t y = 0; y < game.graph().height(); ++y) {
        for (size_t x = 0; x < game.graph().width(); ++x) {
            const auto node = game.graph().node(x, y);
            if (node == nullptr) {
                os << " ";
                continue;
            }
            switch (node->type()) {
            case Board::NodeType::Ocean:
                os << '~';
                continue;
            case Board::NodeType::UnflippedHex:
                os << "?";
                continue;
            case Board::NodeType::Junction: {
                const auto junction = game.junction(node->index());
                const auto owner = junction->owner();
                if (owner != nullptr) {
                    os << owner->index();
                    continue;
                }
                if (junction->is_port()) {
                    os << "*";
                    continue;
                }
                os << ".";
                continue;
            }
            case Board::NodeType::Road: {
                const auto road = game.road(node->index());
                const auto owner = road->owner();
                if (owner != nullptr) {
                    os << owner->index();
                    continue;
                }
                if (game.graph().has_neighbor(*node, Board::Direction::Clock6)
                    || game.graph().has_neighbor(*node, Board::Direction::Clock12)) {
                    os << '|';
                    continue;
                }
                if (game.graph().has_neighbor(*node, Board::Direction::Clock2)
                    || game.graph().has_neighbor(*node, Board::Direction::Clock8)) {
                    os << '/';
                    continue;
                }
                if (game.graph().has_neighbor(*node, Board::Direction::Clock4)
                    || game.graph().has_neighbor(*node, Board::Direction::Clock10)) {
                    os << '\\';
                    continue;
                }
                assert(false);
            }
            case Board::NodeType::Hex: {
                const auto hex = game.hex(node->index());
                if (hex == robber_hex) {
                    os << "X";
                    continue;
                }
                const auto abstract_resource = hex->resource();
                if (std::holds_alternative<NonYieldingResource>(abstract_resource)) {
                    switch (std::get<NonYieldingResource>(abstract_resource)) {
                    case NonYieldingResource::Desert:
                        os << "d";
                        continue;
                    }
                } else {
                    /*
                    const auto roll = hex->roll_number();
                    os << (roll >= 10 ? roll - 10 : roll);
                    continue;
                    */
                    switch (std::get<Resource>(abstract_resource)) {
                    case Resource::Brick:
                        os << "b";
                        continue;
                    case Resource::Ore:
                        os << "o";
                        continue;
                    case Resource::Sheep:
                        os << "s";
                        continue;
                    case Resource::Wheat:
                        os << "h";
                        continue;
                    case Resource::Wood:
                        os << "w";
                        continue;
                    }
                }
                assert(false);
            }
            }
            assert(false);
        }
        if (y < game.graph().height() - 1) {
            os << std::endl;
        }
    }
    return os;
}

void Game::for_each_hex(const std::function<void(const BoardView::Hex*)>& callback) const
{
    for (const auto& node : m_nodes) {
        if (node != nullptr && node->type() == BoardView::NodeView::Type::Hex) {
            callback(static_cast<const BoardView::Hex*>(node));
        }
    }
}

void Game::for_each_junction(const std::function<void(const BoardView::Junction*)>& callback) const
{
    for (const auto& node : m_nodes) {
        if (node != nullptr && node->type() == BoardView::NodeView::Type::Junction) {
            callback(static_cast<const BoardView::Junction*>(node));
        }
    }
}

void Game::for_each_road(const std::function<void(const BoardView::Road*)>& callback) const
{
    for (const auto& node : m_nodes) {
        if (node != nullptr && node->type() == BoardView::NodeView::Type::Road) {
            callback(static_cast<const BoardView::Road*>(node));
        }
    }
}

BoardView::Hex* Game::hex(size_t index)
{
    if (index >= m_nodes.size()) {
        return nullptr;
    }
    auto* node = m_nodes.at(index);
    if (node == nullptr) {
        return nullptr;
    }
    if (node->type() != BoardView::NodeView::Type::Hex) {
        return nullptr;
    }
    return static_cast<BoardView::Hex*>(node);
}

BoardView::Junction* Game::junction(size_t index)
{
    if (index >= m_nodes.size()) {
        return nullptr;
    }
    auto* node = m_nodes.at(index);
    if (node == nullptr) {
        return nullptr;
    }
    if (node->type() != BoardView::NodeView::Type::Junction) {
        return nullptr;
    }
    return static_cast<BoardView::Junction*>(node);
}

BoardView::Road* Game::road(size_t index)
{
    if (index >= m_nodes.size()) {
        return nullptr;
    }
    auto* node = m_nodes.at(index);
    if (node == nullptr) {
        return nullptr;
    }
    if (node->type() != BoardView::NodeView::Type::Road) {
        return nullptr;
    }
    return static_cast<BoardView::Road*>(node);
}

const BoardView::Hex* Game::hex(size_t index) const
{
    if (index >= m_nodes.size()) {
        return nullptr;
    }
    const auto* node = m_nodes.at(index);
    if (node == nullptr) {
        return nullptr;
    }
    if (node->type() != BoardView::NodeView::Type::Hex) {
        return nullptr;
    }
    return static_cast<const BoardView::Hex*>(node);
}

const BoardView::Junction* Game::junction(size_t index) const
{
    if (index >= m_nodes.size()) {
        return nullptr;
    }
    const auto* node = m_nodes.at(index);
    if (node == nullptr) {
        return nullptr;
    }
    if (node->type() != BoardView::NodeView::Type::Junction) {
        return nullptr;
    }
    return static_cast<const BoardView::Junction*>(node);
}

const BoardView::Road* Game::road(size_t index) const
{
    if (index >= m_nodes.size()) {
        return nullptr;
    }
    const auto* node = m_nodes.at(index);
    if (node == nullptr) {
        return nullptr;
    }
    if (node->type() != BoardView::NodeView::Type::Road) {
        return nullptr;
    }
    return static_cast<const BoardView::Road*>(node);
}

void Game::encode(ByteBuffer& buf) const
{
    Encoder encoder(buf);

    encoder << m_owner;
    encoder << m_scenario;
    encoder << m_nodes;
    encoder << m_deck;
    encoder << m_dice;
    encoder << m_robber.location()->index();
    encoder << m_victory_points_goal;
    encoder << m_players;
    encoder << m_deck_index;
    encoder << m_current_player_index;
    encoder << m_can_steal;
    encoder << m_has_rolled;
    encoder << m_is_trade_accepted;
    encoder << m_num_trades_offered_this_turn;

    if (!m_current_trade.has_value()) {
        encoder << false;
    } else {
        encoder << true;
        encoder << m_current_trade->offerer->index();
        encoder << m_current_trade->offered_to.size();
        for (auto* offered_to : m_current_trade->offered_to)
            encoder << offered_to->index();
        encoder << m_current_trade->from_offerer;
        encoder << m_current_trade->to_offerer;
    }

    encoder << m_turn;
    if (m_has_largest_army == nullptr) {
        encoder << false;
    } else {
        encoder << true;
        encoder << m_has_largest_army->index();
    }
    if (m_has_longest_road == nullptr) {
        encoder << false;
    } else {
        encoder << true;
        encoder << m_has_longest_road->index();
    }
    encoder << m_buildings_built;
}

Game* Game::decode(ByteBuffer& buf)
{
    Decoder decoder(buf);

    PlayerId owner;
    if (!decoder.decode(owner))
        return nullptr;

    const Scenario::Scenario* scenario;
    if (!decoder.decode(scenario))
        return nullptr;

    std::vector<BoardView::NodeView*> nodes;
    size_t n_nodes;
    if (!decoder.decode(n_nodes))
        return nullptr;
    nodes.reserve(n_nodes);
    for (size_t i = 0; i < n_nodes; ++i) {
        auto* node = BoardView::NodeView::decode(buf, scenario->graph());
        nodes.push_back(node);
    }

    std::vector<DevelopmentCard> deck;
    if (!decoder.decode(deck))
        return nullptr;

    Dice dice;
    if (!decoder.decode(dice))
        return nullptr;

    size_t robber_location_index;
    if (!decoder.decode(robber_location_index))
        return nullptr;
    auto* robber_node = nodes.at(robber_location_index);
    if (robber_node == nullptr)
        return nullptr;
    if (!robber_node->is_hex())
        return nullptr;
    auto* robber_hex = static_cast<BoardView::Hex*>(robber_node);

    size_t victory_points_goal;
    if (!decoder.decode(victory_points_goal))
        return nullptr;

    std::vector<Player*> players;
    size_t n_players;
    if (!decoder.decode(n_players))
        return nullptr;
    nodes.reserve(n_nodes);
    for (size_t i = 0; i < n_players; ++i) {
        auto* player = Player::decode(buf, nodes);
        players.push_back(player);
    }

    size_t deck_index;
    if (!decoder.decode(deck_index))
        return nullptr;

    size_t current_player_index;
    if (!decoder.decode(current_player_index))
        return nullptr;

    bool can_steal;
    if (!decoder.decode(can_steal))
        return nullptr;

    bool has_rolled;
    if (!decoder.decode(has_rolled))
        return nullptr;

    bool is_trade_accepted;
    if (!decoder.decode(is_trade_accepted))
        return nullptr;

    size_t num_trades_offered_this_turn;
    if (!decoder.decode(num_trades_offered_this_turn))
        return nullptr;

    std::optional<Trade> current_trade;
    bool has_current_trade;
    if (!decoder.decode(has_current_trade))
        return nullptr;
    if (has_current_trade) {
        assert(false);
    }

    size_t turn;
    if (!decoder.decode(turn))
        return nullptr;

    auto* game = new Game(owner, scenario, nodes, deck, victory_points_goal, robber_hex);
    game->m_dice = dice;
    game->m_players = std::move(players);
    for (auto* player : game->players())
        player->set_game(game);
    game->m_deck_index = deck_index;
    game->m_current_player_index = current_player_index;
    game->m_can_steal = can_steal;
    game->m_has_rolled = has_rolled;
    game->m_is_trade_accepted = is_trade_accepted;
    game->m_num_trades_offered_this_turn = num_trades_offered_this_turn;
    game->m_current_trade = std::move(current_trade);
    game->m_turn = turn;
    return game;
}

Game::Game(PlayerId owner,
           const Scenario::Scenario* scenario,
           std::vector<BoardView::NodeView*>& nodes,
           std::vector<DevelopmentCard> deck,
           size_t victory_points_goal,
           BoardView::Hex* robber_location)
    : m_owner(owner)
    , m_scenario(scenario)
    , m_nodes(std::move(nodes))
    , m_deck(std::move(deck))
    , m_robber(robber_location)
    , m_victory_points_goal(victory_points_goal)
{
}

bool Game::operator==(const Game& that) const
{
    if (this->owner() != that.owner())
        return false;

    if (this->can_steal() != that.can_steal())
        return false;
    if (this->has_rolled() != that.has_rolled())
        return false;
    if (this->is_game_over() != that.is_game_over())
        return false;
    if (this->is_trade_accepted() != that.is_trade_accepted())
        return false;
    if (this->is_first_round() != that.is_first_round())
        return false;
    if (this->is_second_round() != that.is_second_round())
        return false;
    if (this->is_roll_seven() != that.is_roll_seven())
        return false;
    if (this->should_wait_for_discard() != that.should_wait_for_discard())
        return false;
    if (this->should_wait_for_trade() != that.should_wait_for_trade())
        return false;

    if (this->num_trades_offered_this_turn() != that.num_trades_offered_this_turn())
        return false;
    if (this->has_current_trade() != that.has_current_trade())
        return false;
    if (!(this->current_trade() == that.current_trade()))
        return false;
    if (this->robber_location()->index() != that.robber_location()->index())
        return false;

    if (this->get_dice_total() != that.get_dice_total())
        return false;

    if (this->turn() != that.turn())
        return false;
    if (this->get_round() != that.get_round())
        return false;

    if (this->largest_army() != that.largest_army())
        return false;
    if (this->longest_road() != that.longest_road())
        return false;

    if (this->has_largest_army() == nullptr) {
        if (that.has_largest_army() != nullptr)
            return false;
    } else {
        if (this->has_largest_army()->index() != that.has_largest_army()->index())
            return false;
    }

    if (this->has_longest_road() == nullptr) {
        if (that.has_longest_road() != nullptr)
            return false;
    } else {
        if (this->has_longest_road()->index() != that.has_longest_road()->index())
            return false;
    }

    for (auto i = 0; i < 4; ++i) {
        if (this->num_built(static_cast<Building>(i)) != that.num_built(static_cast<Building>(i)))
            return false;
    }

    if (Scenario::Store::the().name_of(&this->scenario()) != Scenario::Store::the().name_of(&that.scenario()))
        return false;

    if (this->current_player().index() != that.current_player().index())
        return false;
    if (this->players().size() != that.players().size())
        return false;
    for (size_t i = 0; i < this->players().size(); ++i)
        if (this->players().at(i)->index() != that.players().at(i)->index())
            return false;

    return true;
}

} // namespace k10engine::Game

template<>
void encode(ByteBuffer& buf, const k10engine::Game::Game& game)
{
    game.encode(buf);
}

template<>
bool decode(ByteBuffer& buf, k10engine::Game::Game*& game)
{
    game = k10engine::Game::Game::decode(buf);
    return game != nullptr;
}
