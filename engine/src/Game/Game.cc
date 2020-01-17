#include <assert.h>
#include <variant>

#include "Game/Game.h"

namespace k10engine {

namespace Game {

void Game::roll_dice()
{
    assert(false);
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
    return get_dice_total() == 7;
}

bool Game::should_wait_for_discard()
{
    assert(false);
}

bool Game::should_wait_for_trade()
{
    assert(false);
}

int Game::get_round()
{
    assert(false);
}

std::optional<Game>
Game::initialize(Board::Base* board, Scenario::Scenario* scenario, Scenario::Parameters* parameters)
{
    if (!scenario->is_valid(parameters))
        return std::nullopt;

    auto deck = scenario->get_development_card_deck(parameters->development_card_iteration_type);
    auto ports = scenario->get_ports(parameters->port_iteration_type);
    auto resources = scenario->get_resources(parameters->resource_iteration_type);
    auto rolls = scenario->get_rolls(parameters->roll_iteration_type);

    std::vector<BoardView::Hex*> hex_views;
    int roll_index = 0;
    for (int i = 0; i < board->graph()->n_hexes(); ++i) {
        auto hex = board->graph()->hex(i);
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

    std::vector<BoardView::Junction*> junction_views;
    int port_index = 0;
    std::map<int, ResourceCollection> port_partner_types;
    for (int i = 0; i < board->graph()->n_junctions(); ++i) {
        auto junction = board->graph()->junction(i);
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

    std::vector<BoardView::Road*> road_views;
    for (int i = 0; i < board->graph()->n_roads(); ++i) {
        auto road = board->graph()->road(i);
        auto road_view = new BoardView::Road(road);
        road_views.push_back(road_view);
    }

    assert(false);
}

Game::~Game()
{
    assert(false);
}

} // namespace Game

} // namespace k10engine
