#include <cassert>
#include <iostream>
#include <stdexcept>
#include <variant>

#include "Game/Game.h"

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
        if (player.num_to_discard() > 0) {
            return true;
        }
    }
    return false;
}

bool Game::should_wait_for_trade() const
{
    for (const auto& player : m_players) {
        if (player.can_accept_trade() && !player.has_declined_trade()) {
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
            player.set_can_accept_trade(false);
            player.set_has_declined_trade(false);
        }
    }
}

size_t Game::get_round() const
{
    return m_turn / m_players.size();
}

Game::Game(const Board::Graph* graph,
           std::map<size_t, BoardView::Hex*> hexes,
           std::map<size_t, BoardView::Junction*> junctions,
           std::map<size_t, BoardView::Road*> roads,
           std::vector<DevelopmentCard> deck,
           const Scenario::Scenario& scenario,
           const Scenario::Parameters& parameters,
           int robber_index)
    : m_graph(graph)
    , m_hexes(std::move(hexes))
    , m_junctions(std::move(junctions))
    , m_roads(std::move(roads))
    , m_deck(std::move(deck))
    , m_scenario(scenario)
    , m_robber(m_hexes.at(robber_index))
    , m_victory_points_goal(parameters.victory_points_goal)
{
    for (size_t i = 0; i < parameters.players_count; ++i) {
        m_players.push_back(Player(i, this));
    }
    current_player().set_vertex(State::Vertex::Root);
}

Game::~Game()
{
    for (auto hex : m_hexes) {
        delete hex.second;
    }
    for (auto junction : m_junctions) {
        delete junction.second;
    }
    for (auto road : m_roads) {
        delete road.second;
    }
    m_deck.clear();
}

Game* initialize(const Board::Graph* graph,
                 const Scenario::Scenario& scenario,
                 const Scenario::Parameters& parameters)
{
    if (!scenario.is_valid(parameters)) {
        return nullptr;
    }

    const auto& deck =
        scenario.get_development_card_deck(parameters.development_card_iteration_type);
    const auto& ports = scenario.get_ports(parameters.port_iteration_type);
    const auto& resources = scenario.get_resources(parameters.resource_iteration_type);
    const auto& rolls = scenario.get_rolls(parameters.roll_iteration_type);

    std::map<size_t, BoardView::Hex*> hexes;
    std::map<size_t, BoardView::Junction*> junctions;
    std::map<size_t, BoardView::Road*> roads;

    std::map<const Board::Node*, BoardView::Hex*> hex_lookup;
    std::map<const Board::Node*, BoardView::Junction*> junction_lookup;
    std::map<const Board::Node*, BoardView::Road*> road_lookup;

    int robber_index = -1;

    size_t resource_index = 0;
    size_t roll_index = 0;

    for (const auto& node : graph->nodes()) {
        switch (node.type()) {
        case Board::NodeType::Hex: { // scope for const
            if (resource_index >= resources.size()) {
                return nullptr; // Too few resources
            }
            const auto& resource = resources.at(resource_index);
            BoardView::Hex* hex;
            if (std::holds_alternative<NonYieldingResource>(resource)) {
                if (robber_index != -1) {
                    return nullptr; // FIXME: Handle multiple robbers?
                }
                robber_index = static_cast<int>(node.index());
                hex = new BoardView::Hex(node, resource, 0);
            } else {
                if (roll_index >= rolls.size()) {
                    return nullptr; // Too few rolls
                }
                const auto roll = rolls.at(roll_index);
                ++roll_index;
                hex = new BoardView::Hex(node, resource, roll);
            }
            hex_lookup[&node] = hex;
            hexes[node.index()] = hex;
            ++resource_index;
        } break;
        case Board::NodeType::Junction: { // scope for const
            const auto port = graph->port(node);
            BoardView::Junction* junction;
            if (port == nullptr) {
                junction = new BoardView::Junction(node, {}, 0);
            } else {
                const auto port_index = port->index();
                if (port_index >= ports.size()) {
                    return nullptr; // Too few ports
                }
                const auto& port_spec = ports.at(port_index);
                junction =
                    new BoardView::Junction(node, port_spec.resources, port_spec.exchange_rate);
            }
            junction_lookup[&node] = junction;
            junctions[node.index()] = junction;
        } break;
        case Board::NodeType::Road: { // scope for const
            auto road = new BoardView::Road(node);
            road_lookup[&node] = road;
            roads[node.index()] = road;
        } break;
        case Board::NodeType::Ocean:
        case Board::NodeType::UnflippedHex:
            break; // do nothing
        }
    }

    for (auto hex_it : hexes) {
        const auto hex = hex_it.second;
        for (const auto& direction : Board::AllDirections) {
            const auto neighbor_node = graph->neighbor(hex->node(), direction);
            if (neighbor_node == nullptr) {
                continue;
            }
            if (junction_lookup.find(neighbor_node) != junction_lookup.end()) {
                const auto neighbor_junction = junction_lookup.at(neighbor_node);
                hex->add_neighbor(direction, neighbor_junction);
            }
        }
    }
    for (auto junction_it : junctions) {
        const auto junction = junction_it.second;
        for (const auto& direction : Board::AllDirections) {
            const auto neighbor_node = graph->neighbor(junction->node(), direction);
            if (neighbor_node == nullptr) {
                continue;
            }
            if (hex_lookup.find(neighbor_node) != hex_lookup.end()) {
                const auto neighbor_hex = hex_lookup.at(neighbor_node);
                junction->add_neighbor(direction, neighbor_hex);
            } else if (road_lookup.find(neighbor_node) != road_lookup.end()) {
                const auto neighbor_road = road_lookup.at(neighbor_node);
                junction->add_neighbor(direction, neighbor_road);
            }
        }
    }
    for (auto road_it : roads) {
        const auto road = road_it.second;
        for (const auto& direction : Board::AllDirections) {
            const auto neighbor_node = graph->neighbor(road->node(), direction);
            if (neighbor_node == nullptr) {
                continue;
            }
            if (junction_lookup.find(neighbor_node) != junction_lookup.end()) {
                const auto neighbor_junction = junction_lookup.at(neighbor_node);
                road->add_neighbor(direction, neighbor_junction);
            }
        }
    }

    if (robber_index == -1 || hexes.find(robber_index) == hexes.end()) {
        return nullptr; // something went wrong ... or no Desert given
    }

    return new Game(graph, hexes, junctions, roads, deck, scenario, parameters, robber_index);
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
    for (const auto& junction_neighbor : road->junction_neighbors()) {
        const auto& junction = junction_neighbor.second;
        if (junction->owner() != nullptr) {
            if (*junction->owner() == player) {
                return true;
            }
            continue;
        }
        for (const auto& road_neighbor : junction->road_neighbors()) {
            const auto& other_road = road_neighbor.second;
            if (other_road == road) {
                continue;
            }
            if (other_road->owner() != nullptr && *other_road->owner() == player) {
                return true;
            }
        }
    }
    return false;
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

    return { ResultType::Ok,
             { { ActionArgumentType::DevelopmentCardId, static_cast<size_t>(development_card) } } };
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
            for (const auto& hex_it : settlement->hex_neighbors()) {
                const auto& hex_resource = hex_it.second->resource();
                if (std::holds_alternative<Resource>(hex_resource)) {
                    player.accrue_resources({ { std::get<Resource>(hex_resource), 1 } });
                }
            }
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

Result Game::execute_offer_trade(Player& player, const Trade trade)
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
    for (const auto& junction_it : hex->junction_neighbors()) {
        const auto& junction = junction_it.second;
        if (junction->owner() == nullptr) {
            continue;
        }
        if (*junction->owner() == player) {
            continue;
        }
        set_can_steal(true);
    }
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
        if (other_player == player) {
            continue;
        }
        const auto count = other_player.count(resource);
        other_player.spend_resources({ { resource, count } });
        player.accrue_resources({ { resource, count } });
    }

    player.play_monopoly(resource);
    player.set_vertex(State::Vertex::Root);

    return { ResultType::Ok, {} };
}

Result
Game::execute_play_road_building(Player& player, BoardView::Road* road_0, BoardView::Road* road_1)
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
        for (const auto& junction_it : road_0->junction_neighbors()) {
            if (is_road_1_adjacent_to_road_0) {
                break;
            }
            const auto& junction = junction_it.second;
            const auto& owner = junction->owner();
            if (owner != nullptr && *owner != player) {
                continue;
            }
            for (const auto& road_it : junction->road_neighbors()) {
                const auto& road = road_it.second;
                if (road == road_1) {
                    is_road_1_adjacent_to_road_0 = true;
                    break;
                }
            }
        }
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

Result Game::execute_play_victory_point(Player& player)
{
    if (!player_can_execute_edge(player, State::Edge::PlayVictoryPoint)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    player.play_victory_point();
    player.set_vertex(State::Vertex::Root);

    return { ResultType::Ok, {} };
}

Result Game::execute_play_year_of_plenty(Player& player,
                                         const Resource& resource_0,
                                         const Resource& resource_1)
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
            if (other_player.has_heavy_purse()) {
                size_t num_to_discard = other_player.num_resources() / 2;
                other_player.set_num_to_discard(num_to_discard);
            }
        }
        player.set_vertex(State::Vertex::AfterRollingSeven);
    } else {
        for (const auto& hex_it : hexes()) {
            const auto& hex = hex_it.second;
            if (hex->roll_number() != dice_total) {
                continue;
            }
            const auto& hex_resource = hex->resource();
            if (std::holds_alternative<NonYieldingResource>(hex_resource)) {
                continue;
            }
            const auto& resource = std::get<Resource>(hex_resource);
            for (const auto& junction_neighbor : hex->junction_neighbors()) {
                const auto& junction = junction_neighbor.second;
                if (junction->owner() == nullptr) {
                    continue;
                }
                const auto count = junction->has_city() ? 2 : 1;
                m_players.at(junction->owner()->index()).accrue_resources({ { resource, count } });
            }
        }
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
    for (const auto junction_it : robber_location()->junction_neighbors()) {
        const auto junction_owner = junction_it.second->owner();
        if (junction_owner != nullptr && *junction_owner == steal_from) {
            is_adjacent = true;
            break;
        }
    }
    if (!is_adjacent) {
        return { ResultType::InvalidPlayerId, {} };
    }

    const auto stolen_resource = choose_from(steal_from.m_resources);
    steal_from.spend_resources({ { stolen_resource, 1 } });
    player.accrue_resources({ { stolen_resource, 1 } });

    set_can_steal(false);

    return { ResultType::Ok,
             { { ActionArgumentType::TakeResourceType, static_cast<size_t>(stolen_resource) } } };
}

Result Game::execute_to_root(Player& player)
{
    if (!player_can_execute_edge(player, State::Edge::ToRoot)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    player.set_vertex(State::Vertex::Root);
    return { ResultType::Ok, {} };
}

Result Game::execute_trade_with_bank(Player& player, Trade trade)
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
        if (player.private_victory_points() == m_victory_points_goal) {
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
        for (auto& development_card : player.m_unplayable_development_cards) {
            player.m_playable_development_cards.push_back(development_card);
        }
        player.m_unplayable_development_cards.clear();
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
    for (size_t y = 0; y < game.graph()->height(); ++y) {
        for (size_t x = 0; x < game.graph()->width(); ++x) {
            const auto node = game.graph()->node(x, y);
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
                const auto junction = game.junctions().at(node->index());
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
                const auto road = game.roads().at(node->index());
                const auto owner = road->owner();
                if (owner != nullptr) {
                    os << owner->index();
                    continue;
                }
                if (game.graph()->has_neighbor(*node, Board::Direction::Clock6)
                    || game.graph()->has_neighbor(*node, Board::Direction::Clock12)) {
                    os << '|';
                    continue;
                }
                if (game.graph()->has_neighbor(*node, Board::Direction::Clock2)
                    || game.graph()->has_neighbor(*node, Board::Direction::Clock8)) {
                    os << '/';
                    continue;
                }
                if (game.graph()->has_neighbor(*node, Board::Direction::Clock4)
                    || game.graph()->has_neighbor(*node, Board::Direction::Clock10)) {
                    os << '\\';
                    continue;
                }
                assert(false);
            }
            case Board::NodeType::Hex: {
                const auto hex = game.hexes().at(node->index());
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
        if (y < game.graph()->height() - 1) {
            os << std::endl;
        }
    }
    return os;
}

} // namespace k10engine::Game
