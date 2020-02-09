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
    (void)ports;
    auto resources = scenario.get_resources(parameters.resource_iteration_type);
    (void)resources;
    auto rolls = scenario.get_rolls(parameters.roll_iteration_type);
    (void)rolls;

    std::vector<BoardView::Hex> hexes;
    std::vector<BoardView::Junction> junctions;
    std::vector<BoardView::Road> roads;

    for (const auto node : *graph) {
        std::cout << node->type() << std::endl;
    }

    return nullptr;

    /*
    std::cout << "initializing hex views" << std::endl;
    std::vector<BoardView::Hex*> hex_views;
    int roll_index = 0;
    std::cout << "getting n_hexes" << std::endl;
    std::cout << graph->n_hexes() << std::endl;
    std::cout << "got n_hexes" << std::endl;
    for (int i = 0; i < graph->n_hexes(); ++i) {
        std::cout << i << " ";
        auto hex = graph->hex(i);
        auto resource = resources[i];
        BoardView::Hex* hex_view;
        if (std::get_if<Resource>(&resource)) {
            auto roll = rolls[roll_index];
            ++roll_index;
            hex_view = new BoardView::Hex(hex, resource, roll);
        } else {
            hex_view = new BoardView::Hex(hex, resource, std::nullopt);
        }
        hex_views.push_back(hex_view);
    }

    std::cout << std::endl;

    std::cout << "initializing junction views" << std::endl;
    std::vector<BoardView::Junction*> junction_views;
    int port_index = 0;
    std::map<int, ResourceCollection> port_partner_types;
    for (int i = 0; i < graph->n_junctions(); ++i) {
        auto junction = graph->junction(i);
        BoardView::Junction* junction_view;
        if (junction->is_port()) {
            int partner_index = (*junction->port_partner())->index();
            ResourceCollection port_type;
            if (junction->index() < partner_index) {
                port_type = ports[port_index];
                ++port_index;
                port_partner_types[partner_index] = port_type;
            } else {
                port_type = port_partner_types[partner_index];
            }
            junction_view = new BoardView::Junction(junction, port_type);
        } else {
            junction_view = new BoardView::Junction(junction, std::nullopt);
        }
        junction_views.push_back(junction_view);
    }

    std::cout << "initializing road views" << std::endl;
    std::vector<BoardView::Road*> road_views;
    for (int i = 0; i < graph->n_roads(); ++i) {
        auto road = graph->road(i);
        auto road_view = new BoardView::Road(road);
        road_views.push_back(road_view);
    }

    std::cout << "got to end" << std::endl;
    throw std::invalid_argument("Not implemented");
    */
}

} // namespace k10engine::Game
