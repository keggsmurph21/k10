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
    return m_turn % players().size();
}

Game::Game(const Board::Graph* graph,
           std::vector<BoardView::Hex*> hexes,
           std::vector<BoardView::Junction*> junctions,
           std::vector<BoardView::Road*> roads,
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
        delete hex;
    }
    for (auto junction : m_junctions) {
        delete junction;
    }
    for (auto road : m_roads) {
        delete road;
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

    std::vector<BoardView::Hex*> hexes;
    std::vector<BoardView::Junction*> junctions;
    std::vector<BoardView::Road*> roads;

    std::map<const Board::Node*, BoardView::Hex*> hex_lookup;
    std::map<const Board::Node*, BoardView::Junction*> junction_lookup;
    std::map<const Board::Node*, BoardView::Road*> road_lookup;

    int robber_index = -1;

    size_t hex_index = 0;
    size_t roll_index = 0;

    for (const auto& node : graph->nodes()) {
        switch (node->type()) {
        case Board::NodeType::Hex: { // scope for const
            if (hex_index >= resources.size()) {
                return nullptr; // Too few resources
            }
            const auto& resource = resources.at(hex_index);
            BoardView::Hex* hex;
            if (std::holds_alternative<NonYieldingResource>(resource)) {
                if (robber_index != -1) {
                    return nullptr; // FIXME: Handle multiple robbers?
                }
                robber_index = static_cast<int>(hex_index);
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
            hexes.push_back(hex);
            ++hex_index;
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
            junctions.push_back(junction);
        } break;
        case Board::NodeType::Road: { // scope for const
            auto road = new BoardView::Road(node);
            road_lookup[node] = road;
            roads.push_back(road);
        } break;
        case Board::NodeType::Ocean:
        case Board::NodeType::UnflippedHex:
            break; // do nothing
        }
    }

    for (auto hex : hexes) {
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
    for (auto junction : junctions) {
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
    for (auto road : roads) {
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

    if (!(0 <= robber_index && robber_index < static_cast<int>(hexes.size()))) {
        return nullptr; // something went wrong ... or no Desert given
    }

    return new Game(graph, hexes, junctions, roads, deck, scenario, parameters, robber_index);
}

Result Game::execute_action(size_t player_id, const Action& action)
{
    (void)player_id;
    (void)action;
    throw std::invalid_argument("Not implemented: execute_action()");
}

} // namespace k10engine::Game
