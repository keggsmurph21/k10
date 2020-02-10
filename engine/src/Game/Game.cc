#include <iostream>
#include <stdexcept>
#include <variant>

#include "Game/Game.h"

namespace k10engine::Game {

void Game::roll_dice() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Game::roll_dice");
}

int Game::get_dice_total()
{
    return m_dice.get_total();
}

bool Game::is_first_round()
{
    return get_round() == 0;
}

bool Game::is_second_round()
{
    return get_round() == 1;
}

bool Game::is_roll_seven()
{
    return get_dice_total() == k10_ROBBER_DICE_ROLL;
}

bool Game::should_wait_for_discard() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Game::should_wait_for_discard");
}

bool Game::should_wait_for_trade() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Game::should_wait_for_trade");
}

int Game::get_round() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Game::get_round");
}

Game::Game(std::vector<BoardView::Hex> hexes,
           std::vector<BoardView::Junction> junctions,
           std::vector<BoardView::Road> roads,
           std::list<DevelopmentCard> deck,
           const Scenario::Scenario& scenario)
    : m_hexes(std::move(hexes))
    , m_junctions(std::move(junctions))
    , m_roads(std::move(roads))
    , m_deck(std::move(deck))
    , m_scenario(scenario)
{
    m_dice = Dice();
    m_robber = Robber();
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

    auto deck = scenario.get_development_card_deck(parameters.development_card_iteration_type);
    (void)deck;
    auto ports = scenario.get_ports(parameters.port_iteration_type);
    auto resources = scenario.get_resources(parameters.resource_iteration_type);
    auto rolls = scenario.get_rolls(parameters.roll_iteration_type);

    std::vector<BoardView::Hex> hexes;
    std::vector<BoardView::Junction> junctions;
    std::vector<BoardView::Road> roads;

    int robber_index = -1; // FIXME: Pass this along to Game contructor

    size_t hex_index = 0;
    size_t roll_index = 0;

    for (const auto node : *graph) {
        std::cout << node->index() << " " << node->type();
        switch (node->type()) {
        case Board::NodeType::Hex: { // scope for const
            std::cout << " " << hex_index;
            if (hex_index >= resources.size()) {
                return nullptr; // Too few resources
            }
            const auto resource = *resources.at(hex_index);
            std::cout << " " << resource;
            if (std::holds_alternative<NonYieldingResource>(resource)) {
                if (robber_index != -1) {
                    return nullptr; // FIXME: Handle multiple robbers?
                }
                robber_index = static_cast<int>(hex_index);
                const auto hex = BoardView::Hex(node, resource, 0);
                hexes.push_back(hex);
            } else {
                if (roll_index >= rolls.size()) {
                    return nullptr; // Too few rolls
                }
                const auto roll = rolls.at(roll_index);
                std::cout << " " << roll;
                ++roll_index;
                const auto hex = BoardView::Hex(node, resource, roll);
                hexes.push_back(hex);
            }
            ++hex_index;
        } break;
        case Board::NodeType::Junction: { // scope for const
            const auto port = graph->port(*node);
            if (port == nullptr) {
                break;
            }
            const auto port_index = port->index();
            if (port_index >= ports.size()) {
                return nullptr; // Too few ports
            }
            const auto port_spec = ports.at(port_index);
            const auto other_node = port->buddy(node);
            std::cout << " Port " << port_index << " " << port_spec.resources << " @ "
                      << port_spec.exchange_rate << " (with " << other_node->index() << ")";
            auto junction = BoardView::Junction(node, port_spec.resources, port_spec.exchange_rate);
            junctions.push_back(junction);
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

    return nullptr;
}

} // namespace k10engine::Game
