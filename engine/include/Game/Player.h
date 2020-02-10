#pragma once

#include <string.h>
#include <vector>

#include "Core/DevelopmentCard.h"
#include "Core/Resource.h"
#include "Core/Vertex.h"
#include "Game/BoardView/Junction.h"
#include "Game/BoardView/Road.h"
#include "Game/Flags.h"
#include "Game/Game.h"
#include "Game/Trade.h"

namespace k10engine {

namespace Game {

class Game;

class Player {
public:
    size_t id() const { return m_id; }

    Flags get_flags() const;

    bool can_accept_trade() const { return m_can_accept_trade; }
    void set_can_accept_trade(bool can_accept_trade) { m_can_accept_trade = can_accept_trade; }

    bool has_declined_trade() const { return m_has_declined_trade; }
    void set_has_declined_trade(bool has_declined_trade)
    {
        m_has_declined_trade = has_declined_trade;
    }

    int num_to_discard() const { return m_num_to_discard; }
    void set_num_to_discard(int num_to_discard) { m_num_to_discard = num_to_discard; }

    State::Vertex& vertex() { return m_vertex; }
    void set_vertex(State::Vertex vertex) { m_vertex = vertex; }

    bool has_heavy_purse() const;
    bool can_build_city() const;
    bool can_build_road() const;
    bool can_build_settlement() const;
    bool can_buy_development_card() const;
    bool can_play_knight() const;
    bool can_play_monopoly() const;
    bool can_play_road_building() const;
    bool can_play_victory_point() const;
    bool can_play_year_of_plenty() const;
    bool can_trade() const;
    bool can_trade_with_bank() const;
    bool is_current_player() const;
    bool is_blocking() const;

    bool can_afford(const ResourceCounts&) const;

    int army_size() const;
    int longest_road() const;

    int public_victory_points() const;
    int private_victory_points() const;

    ~Player() {}

    friend Game;

private:
    Player(size_t id, const Game* game)
        : m_id(id)
        , m_game(game)
    {
    }

    size_t m_id;
    const Game* m_game;

    ResourceCounts m_resources;

    std::vector<BoardView::Junction> m_cities;
    std::vector<BoardView::Road> m_roads;
    std::vector<BoardView::Junction> m_settlements;

    std::vector<DevelopmentCard> m_played_development_cards;
    std::vector<DevelopmentCard> m_playable_development_cards;
    std::vector<DevelopmentCard> m_unplayable_development_cards;

    State::Vertex m_vertex{ State::Vertex::Root };

    bool m_can_accept_trade{ false };
    bool m_has_declined_trade{ false };
    int m_num_to_discard{ 0 };
    int m_public_victory_points{ 0 };
    int m_private_victory_points{ 0 };

    void accrue_resources(ResourceCounts);
    void spend_resources(ResourceCounts);
    void collect_resource(Resource);
    void build_road(BoardView::Road);
    void build_road_no_cost(BoardView::Road);
    void build_settlement(BoardView::Junction);
    void build_settlement_no_cost(BoardView::Junction);
    void build_city(BoardView::Junction);

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

} // namespace Game

} // namespace k10engine
