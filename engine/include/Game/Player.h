#pragma once

#include <iostream>
#include <string.h>
#include <vector>

#include "Core/Action.h"
#include "Core/Building.h"
#include "Core/DevelopmentCard.h"
#include "Core/Edge.h"
#include "Core/Resource.h"
#include "Core/Vertex.h"
#include "Game/BoardView/Junction.h"
#include "Game/BoardView/Road.h"
#include "Game/Game.h"
#include "Game/Trade.h"

namespace k10engine::Game {

#define MAX_NUM_TRADE_OFFERS_PER_TURN 42

class Game;

class Player {
public:
    size_t index() const { return m_index; }

    std::vector<Action> get_available_actions() const;

    bool can_accept_trade() const { return m_can_accept_trade; }
    void set_can_accept_trade(bool can_accept_trade) { m_can_accept_trade = can_accept_trade; }

    bool has_declined_trade() const { return m_has_declined_trade; }
    void set_has_declined_trade(bool has_declined_trade)
    {
        m_has_declined_trade = has_declined_trade;
    }

    size_t num_to_discard() const { return m_num_to_discard; }
    void set_num_to_discard(size_t num_to_discard) { m_num_to_discard = num_to_discard; }

    const State::Vertex& vertex() const { return m_vertex; }

    bool has_heavy_purse() const;
    bool can_build(const Building&) const;
    bool can_play(const DevelopmentCard&) const;
    bool can_trade() const;
    bool can_trade_with_bank() const;
    bool is_current_player() const;
    bool is_blocking() const;

    bool can_afford(const ResourceCounts&) const;

    size_t army_size() const;
    size_t longest_road() const;

    size_t public_victory_points() const;
    size_t private_victory_points() const;

    const std::vector<BoardView::Road*>& roads() const { return m_roads; }

    ~Player() {}

    friend Game;
    friend std::ostream& operator<<(std::ostream&, const Player&);

private:
    Player(size_t index, const Game* game)
        : m_index(index)
        , m_game(game)
    {
    }

    size_t m_index;
    const Game* m_game;

    ResourceCounts m_resources;

    std::vector<BoardView::Junction*> m_cities;
    std::vector<BoardView::Road*> m_roads;
    std::vector<BoardView::Junction*> m_settlements;

    std::vector<DevelopmentCard> m_played_development_cards;
    std::vector<DevelopmentCard> m_playable_development_cards;
    std::vector<DevelopmentCard> m_unplayable_development_cards;

    State::Vertex m_vertex{ State::Vertex::WaitForTurn };

    bool m_can_accept_trade{ false };
    bool m_has_declined_trade{ false };
    size_t m_num_to_discard{ 0 };
    size_t m_public_victory_points{ 0 };
    size_t m_private_victory_points{ 0 };
    size_t m_num_trades_offered_this_turn{ 0 };

    void set_vertex(State::Vertex vertex) { m_vertex = vertex; }

    void accrue_resources(ResourceCounts);
    void spend_resources(ResourceCounts);
    void collect_resource(Resource);
    void build_road(BoardView::Road*);
    void build_road_no_cost(BoardView::Road*);
    void build_settlement(BoardView::Junction*);
    void build_settlement_no_cost(BoardView::Junction*);
    void build_city(BoardView::Junction*);

    void update_longest_road();
    void update_victory_points();

    void accept_trade_as_offerer();
    void accept_trade_as_acceptor();
    void cancel_trade();
    void decline_trade();
    void fail_trade(); // unable to find a trade partner
    void trade_with_bank();

    void buy_development_card(DevelopmentCard);

    void move_robber();

    void steal_from(Player*);
};

} // namespace k10engine::Game
