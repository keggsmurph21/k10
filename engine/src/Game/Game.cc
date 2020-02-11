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

int Game::get_round() const // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Game::get_round");
}

Game::Game(const Board::Graph* graph,
           std::vector<BoardView::Hex> hexes,
           std::vector<BoardView::Junction> junctions,
           std::vector<BoardView::Road> roads,
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
    , m_robber(&m_hexes.at(robber_index))
    , m_victory_points_goal(parameters.victory_points_goal)
{
    for (int i = 0; i < parameters.players_count; ++i) {
        m_players.push_back(new Player(i, this));
    }
}

Game::~Game()
{
    m_hexes.clear();
    m_junctions.clear();
    m_roads.clear();
    m_deck.clear();

    for (auto player : m_players) {
        delete player;
    }
    m_players.clear();
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

    std::vector<BoardView::Hex> hexes;
    std::vector<BoardView::Junction> junctions;
    std::vector<BoardView::Road> roads;

    int robber_index = -1;

    size_t hex_index = 0;
    size_t roll_index = 0;

    for (const auto& node : graph->nodes()) {
        std::cout << node->index() << " " << node->type();
        switch (node->type()) {
        case Board::NodeType::Hex: { // scope for const
            std::cout << " " << hex_index;
            if (hex_index >= resources.size()) {
                return nullptr; // Too few resources
            }
            const auto& resource = resources.at(hex_index);
            std::cout << " " << resource;
            if (std::holds_alternative<NonYieldingResource>(resource)) {
                if (robber_index != -1) {
                    return nullptr; // FIXME: Handle multiple robbers?
                }
                robber_index = static_cast<int>(hex_index);
                const auto& hex = BoardView::Hex(node, resource, 0);
                hexes.push_back(hex);
            } else {
                if (roll_index >= rolls.size()) {
                    return nullptr; // Too few rolls
                }
                const auto roll = rolls.at(roll_index);
                std::cout << " " << roll;
                ++roll_index;
                const auto& hex = BoardView::Hex(node, resource, roll);
                hexes.push_back(hex);
            }
            ++hex_index;
        } break;
        case Board::NodeType::Junction: { // scope for const
            const auto port = graph->port(*node);
            if (port == nullptr) {
                auto junction = BoardView::Junction(node, {}, 0);
                junctions.push_back(junction);
            } else {
                const auto port_index = port->index();
                if (port_index >= ports.size()) {
                    return nullptr; // Too few ports
                }
                const auto& port_spec = ports.at(port_index);
                std::cout << " Port " << port_index << " " << port_spec.resources << " @ "
                          << port_spec.exchange_rate;
                auto junction =
                    BoardView::Junction(node, port_spec.resources, port_spec.exchange_rate);
                junctions.push_back(junction);
            }
        } break;
        case Board::NodeType::Road: { // scope for const
            const auto road = BoardView::Road(node);
            roads.push_back(road);
        } break;
        default:
            break; // do nothing
        }
        std::cout << std::endl;
    }

    if (!(0 <= robber_index && robber_index < static_cast<int>(hexes.size()))) {
        return nullptr; // something went wrong ... or no Desert given
    }

    return new Game(graph, hexes, junctions, roads, deck, scenario, parameters, robber_index);
}

} // namespace k10engine::Game
