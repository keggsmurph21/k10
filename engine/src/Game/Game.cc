#include <cassert>
#include <iostream>
#include <stdexcept>
#include <variant>

#include "Game/Game.h"

namespace k10engine::Game {

void Game::roll_dice() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Game::roll_dice");
}

int Game::get_dice_total() const
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

int Game::get_round() const
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
    for (int i = 0; i < parameters.players_count; ++i) {
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
        throw std::invalid_argument("Not implemented: execution_action(State::Edge::AcceptTrade)");

    case State::Edge::Build:
        if (action.args.size() != 2) {
            return { ResultType::InvalidNumberOfArgs, {} };
        }
        if (action.args.at(0).type != ActionArgumentType::BuildItemId) {
            return { ResultType::InvalidArgumentType, {} };
        }
        {
            const auto build_item = static_cast<Building>(action.args.at(0).value);
            switch (build_item) {

            case Building::City:
                assert(false);
                return { ResultType::Ok, {} };

            case Building::DevelopmentCard:
                assert(false);
                return { ResultType::Ok, {} };

            case Building::Road: {
                assert(false);
                return { ResultType::Ok, {} };
            }

            case Building::Settlement: {

                // FIXME: This should be abstracted to a ::parse_junction() helper
                if (action.args.at(1).type != ActionArgumentType::NodeId) {
                    return { ResultType::InvalidArgumentType, {} };
                }
                const auto chosen_node = m_graph->node(action.args.at(1).value);
                if (chosen_node == nullptr) {
                    return { ResultType::NodeIdOutOfRange, {} };
                }
                const auto chosen_junction_entry = m_junctions.find(chosen_node->index());
                if (chosen_junction_entry == m_junctions.end()) {
                    return { ResultType::InvalidNodeId, {} };
                }
                const auto chosen_junction = chosen_junction_entry->second;
                if (!chosen_junction->is_settleable()) {
                    return { ResultType::JunctionNotSettleable, {} };
                }

                if (is_first_round() || is_second_round()) {
                    build_settlement(player, chosen_junction, Options::NoCost);
                } else {
                    build_settlement(player, chosen_junction, Options::None);
                }

                if (is_game_over()) {
                    std::invalid_argument("Not implemented: setting game over");
                } else if (is_first_round()) {
                    player->set_vertex(State::Vertex::WaitForTurn);
                } else if (is_second_round()) {
                    player->set_vertex(State::Vertex::WaitForTurn);
                } else {
                    player->set_vertex(State::Vertex::Root);
                    assert(false);
                }

                return { ResultType::Ok, {} };
            }

            default:
                return { ResultType::BuildingIdOutOfRange, {} };
            }
        }

    case State::Edge::CancelTrade:
        throw std::invalid_argument("Not implemented: execution_action(State::Edge::CancelTrade)");

    case State::Edge::CollectResources:
        throw std::invalid_argument(
            "Not implemented: execution_action(State::Edge::CollectResources)");

    case State::Edge::DeclineTrade:
        throw std::invalid_argument("Not implemented: execution_action(State::Edge::DeclineTrade)");

    case State::Edge::Discard:
        throw std::invalid_argument("Not implemented: execution_action(State::Edge::Discard)");

    case State::Edge::FailTradeUnableToFindPartner:
        throw std::invalid_argument(
            "Not implemented: execution_action(State::Edge::FailTradeUnableToFindPartner)");

    case State::Edge::MoveRobber:
        throw std::invalid_argument("Not implemented: execution_action(State::Edge::MoveRobber)");

    case State::Edge::OfferTrade:
        throw std::invalid_argument("Not implemented: execution_action(State::Edge::OfferTrade)");

    case State::Edge::PlayDevelopmentCard:
        throw std::invalid_argument(
            "Not implemented: execution_action(State::Edge::PlayDevelopmentCard)");

    case State::Edge::RollDice:
        throw std::invalid_argument("Not implemented: execution_action(State::Edge::RollDice)");

    case State::Edge::Steal:
        throw std::invalid_argument("Not implemented: execution_action(State::Edge::Steal)");

    case State::Edge::ToRoot:
        player->set_vertex(State::Vertex::Root);
        return { ResultType::Ok, {} };
    }
    assert(false);
}

void Game::build_settlement(Player* player, BoardView::Junction* junction, Options options)
{
    player->build_settlement(junction, options);
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
    if (m_turn == m_players.size()) {
        // The first round is now over.  Keep m_current_player_index the same.
    } else if (m_turn == m_players.size() * 2) {
        // The second round is now over.  Keep m_current_player_index the same.
    } else if (is_second_round()) {
        // It's the second round, so we iterate backwards.
        --m_current_player_index;
    } else {
        // The default case.
        ++m_current_player_index;
    }
}

} // namespace k10engine::Game
