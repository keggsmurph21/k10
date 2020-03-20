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

bool Game::should_wait_for_discard() const // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Game::should_wait_for_discard");
}

bool Game::should_wait_for_trade() const // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Game::should_wait_for_trade");
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
        m_players.push_back(new Player(i, this));
    }
    current_player()->set_vertex(State::Vertex::Root);
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
    for (auto player : m_players) {
        delete player;
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
        switch (node->type()) {
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
                robber_index = static_cast<int>(node->index());
                hex = new BoardView::Hex(node, resource, 0);
            } else {
                if (roll_index >= rolls.size()) {
                    return nullptr; // Too few rolls
                }
                const auto roll = rolls.at(roll_index);
                ++roll_index;
                hex = new BoardView::Hex(node, resource, roll);
            }
            hex_lookup[node] = hex;
            hexes[node->index()] = hex;
            ++resource_index;
        } break;
        case Board::NodeType::Junction: { // scope for const
            const auto port = graph->port(*node);
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
            junction_lookup[node] = junction;
            junctions[node->index()] = junction;
        } break;
        case Board::NodeType::Road: { // scope for const
            auto road = new BoardView::Road(node);
            road_lookup[node] = road;
            roads[node->index()] = road;
        } break;
        case Board::NodeType::Ocean:
        case Board::NodeType::UnflippedHex:
            break; // do nothing
        }
    }

    for (auto hex_entry : hexes) {
        const auto hex = hex_entry.second;
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
    for (auto junction_entry : junctions) {
        const auto junction = junction_entry.second;
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
    for (auto road_entry : roads) {
        const auto road = road_entry.second;
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

static bool player_can_execute_edge(const Player* player, const Action& requested_action)
{
    const auto valid_actions = player->get_available_actions();
    for (const auto& valid_action : valid_actions) {
        if (valid_action.edge == requested_action.edge) {
            return true;
        }
    }
    return false;
}

static BoardView::Road* parse_road(const Game* game, const ActionArgument& arg)
{
    if (arg.type != ActionArgumentType::NodeId) {
        return nullptr;
    }
    const auto node = game->graph()->node(arg.value);
    if (node == nullptr) {
        return nullptr;
    }
    const auto road_it = game->roads().find(node->index());
    if (road_it == game->roads().end()) {
        return nullptr;
    }
    return road_it->second;
}

static BoardView::Junction* parse_junction(const Game* game, const ActionArgument& arg)
{
    if (arg.type != ActionArgumentType::NodeId) {
        return nullptr;
    }
    const auto node = game->graph()->node(arg.value);
    if (node == nullptr) {
        return nullptr;
    }
    const auto junction_it = game->junctions().find(node->index());
    if (junction_it == game->junctions().end()) {
        return nullptr;
    }
    return junction_it->second;
}

Result Game::execute_accept_trade(Player*, const Action&)
{
    assert(false);
}

Result Game::execute_build_city(Player*, const ActionArgument&)
{
    assert(false);
}

Result Game::execute_build_development_card(Player*, const ActionArgument&)
{
    assert(false);
}

Result Game::execute_build_road(Player* player, const ActionArgument& arg)
{
    const auto road = parse_road(this, arg);
    if (road == nullptr) {
        return { ResultType::InvalidNodeId, {} };
    }

    // FIXME: Need to make sure it's adjacent to something we own!

    if (is_first_round() || is_second_round()) {
        if (player->vertex() == State::Vertex::AfterBuildingFreeSettlement) {
            build_road(player, road, Options::NoCost);
        } else {
            return { ResultType::InvalidEdgeChoice, {} };
        }
    } else if (player->can_afford(Building::Road)) {
        build_road(player, road, Options::None);
    } else {
        return { ResultType::CannotAfford, {} };
    }

    if (is_game_over()) {
        assert(false);
    } else if (is_first_round()) {
        player->set_vertex(State::Vertex::WaitForTurn);
        increment_turn();
    } else if (is_second_round()) {
        player->set_vertex(State::Vertex::ChooseInitialResources);
    } else {
        player->set_vertex(State::Vertex::Root);
        assert(false);
    }

    return { ResultType::Ok, {} };
}

Result Game::execute_build_settlement(Player* player, const ActionArgument& arg)
{
    const auto junction = parse_junction(this, arg);
    if (junction == nullptr) {
        return { ResultType::InvalidNodeId, {} };
    }
    if (!junction->is_settleable()) {
        return { ResultType::JunctionNotSettleable, {} };
    }

    if (is_first_round() || is_second_round()) {
        if (player->vertex() == State::Vertex::Root) {
            build_settlement(player, junction, Options::NoCost);
        } else {
            return { ResultType::InvalidEdgeChoice, {} };
        }
    } else if (player->can_afford(Building::Road)) {
        if (player->can_afford(Building::Settlement)) {
            build_settlement(player, junction, Options::None);
        } else {
            return { ResultType::CannotAfford, {} };
        }
    }

    if (is_game_over()) {
        assert(false);
    } else if (is_first_round() || is_second_round()) {
        player->set_vertex(State::Vertex::AfterBuildingFreeSettlement);
    } else {
        player->set_vertex(State::Vertex::Root);
        assert(false);
    }

    return { ResultType::Ok, {} };
}

Result Game::execute_build(Player* player, const Action& action)
{
    if (action.args.size() != 2) {
        return { ResultType::InvalidNumberOfArgs, {} };
    }
    if (action.args.at(0).type != ActionArgumentType::BuildItemId) {
        return { ResultType::InvalidArgumentType, {} };
    }
    const auto build_item = static_cast<Building>(action.args.at(0).value);
    switch (build_item) {
    case Building::City:
        return execute_build_city(player, action.args.at(1));
    case Building::DevelopmentCard:
        return execute_build_development_card(player, action.args.at(1));
    case Building::Road:
        return execute_build_road(player, action.args.at(1));
    case Building::Settlement:
        return execute_build_settlement(player, action.args.at(1));
    default:
        return { ResultType::BuildingIdOutOfRange, {} };
    }
}

Result Game::execute_cancel_trade(Player*, const Action&)
{
    assert(false);
}

Result Game::execute_choose_initial_resources(Player* player, const Action& action)
{
    if (action.args.size() != 1) {
        return { ResultType::InvalidNumberOfArgs, {} };
    }
    if (action.args.at(0).type != ActionArgumentType::NodeId) {
        return { ResultType::InvalidArgumentType, {} };
    }
    const auto junction = parse_junction(this, action.args.at(0));
    if (junction == nullptr) {
        return { ResultType::InvalidNodeId, {} };
    }
    for (const auto& settlement : player->settlements()) {
        if (settlement == junction) {
            for (const auto& hex_entry : settlement->hex_neighbors()) {
                const auto& hex_resource = hex_entry.second->resource();
                if (std::holds_alternative<Resource>(hex_resource)) {
                    player->accrue_resources({ { std::get<Resource>(hex_resource), 1 } });
                }
            }
            player->set_vertex(State::Vertex::WaitForTurn);
            increment_turn();
            return { ResultType::Ok, {} };
        }
    }
    return { ResultType::InvalidNodeId, {} };
}

Result Game::execute_collect_resources(Player*, const Action&)
{
    assert(false);
}

Result Game::execute_decline_trade(Player*, const Action&)
{
    assert(false);
}

Result Game::execute_discard(Player*, const Action&)
{
    assert(false);
}

Result Game::execute_end_turn(Player* player, const Action&)
{
    player->set_vertex(State::Vertex::WaitForTurn);
    increment_turn();
    return { ResultType::Ok, {} };
}

Result Game::execute_fail_trade_unable_to_find_partner(Player*, const Action&)
{
    assert(false);
}

Result Game::execute_move_robber(Player*, const Action&)
{
    assert(false);
}

Result Game::execute_offer_trade(Player*, const Action&)
{
    assert(false);
}

Result Game::execute_play_development_card(Player*, const Action&)
{
    assert(false);
}

Result Game::execute_roll_dice(Player*, const Action& action)
{
    if (action.args.empty()) {
        m_dice.roll();
#ifdef k10_ENABLE_ROLL_DICE_EXACT
    } else if (action.args.size() == 1) {
        if (action.args.at(0).type != ActionArgumentType::DiceRoll) {
            return { ResultType::InvalidArgumentType, {} };
        }
        const auto roll = action.args.at(0).value;
        if (roll < 2 || 12 < roll) {
            return { ResultType::DiceRollOutOfRange, {} };
        }
        m_dice.set_total(roll);
#endif
    } else {
        return { ResultType::InvalidNumberOfArgs, {} };
    }

    m_has_rolled = true;

    const auto dice_total = get_dice_total();
    for (const auto& hex_entry : hexes()) {
        const auto& hex = hex_entry.second;
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
            m_players.at(junction->owner()->index())->accrue_resources({ { resource, count } });
        }
    }

    return { ResultType::Ok, { { ActionArgumentType::DiceRoll, dice_total } } };
}

Result Game::execute_steal(Player*, const Action&)
{
    assert(false);
}

Result Game::execute_to_root(Player* player, const Action&)
{
    player->set_vertex(State::Vertex::Root);
    return { ResultType::Ok, {} };
}

Result Game::execute_action(size_t player_id, const Action& action)
{
    if (player_id >= m_players.size()) {
        return { ResultType::InvalidPlayerId, {} };
    }
    const auto player = m_players.at(player_id);

    if (!player_can_execute_edge(player, action)) {
        return { ResultType::InvalidEdgeChoice, {} };
    }

    switch (action.edge) {
    case State::Edge::AcceptTrade:
        return execute_accept_trade(player, action);
    case State::Edge::Build:
        return execute_build(player, action);
    case State::Edge::CancelTrade:
        return execute_cancel_trade(player, action);
    case State::Edge::ChooseInitialResources:
        return execute_choose_initial_resources(player, action);
    case State::Edge::CollectResources:
        return execute_collect_resources(player, action);
    case State::Edge::DeclineTrade:
        return execute_decline_trade(player, action);
    case State::Edge::Discard:
        return execute_discard(player, action);
    case State::Edge::EndTurn:
        return execute_end_turn(player, action);
    case State::Edge::FailTradeUnableToFindPartner:
        return execute_fail_trade_unable_to_find_partner(player, action);
    case State::Edge::MoveRobber:
        return execute_move_robber(player, action);
    case State::Edge::OfferTrade:
        return execute_offer_trade(player, action);
    case State::Edge::PlayDevelopmentCard:
        return execute_play_development_card(player, action);
    case State::Edge::RollDice:
        return execute_roll_dice(player, action);
    case State::Edge::Steal:
        return execute_steal(player, action);
    case State::Edge::ToRoot:
        return execute_to_root(player, action);
    }

    return { ResultType::InvalidEdgeChoice, {} };
}

void Game::increment_num_built(Building building)
{
    if (m_buildings_built.find(building) == m_buildings_built.end()) {
        m_buildings_built[building] = 0;
    }
    m_buildings_built[building] += 1;
}

void Game::build_settlement(Player* player, BoardView::Junction* junction, Options options)
{
    player->build_settlement(junction, options);
    increment_num_built(Building::Settlement);
}

void Game::build_road(Player* player, BoardView::Road* road, Options options)
{
    player->build_road(road, options);
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
}

int Game::largest_army() const
{
    if (m_has_largest_army == nullptr) {
        return k10_LARGEST_ARMY_THRESHOLD;
    }
    return m_has_largest_army->army_size();
}

int Game::longest_road() const
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
                if (game.graph()->has_neighbor(node, Board::Direction::Clock6)
                    || game.graph()->has_neighbor(node, Board::Direction::Clock12)) {
                    os << '|';
                    continue;
                }
                if (game.graph()->has_neighbor(node, Board::Direction::Clock2)
                    || game.graph()->has_neighbor(node, Board::Direction::Clock8)) {
                    os << '/';
                    continue;
                }
                if (game.graph()->has_neighbor(node, Board::Direction::Clock4)
                    || game.graph()->has_neighbor(node, Board::Direction::Clock10)) {
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
