#pragma once

#include <string.h>
#include <vector>

#include "Game/Flags.h"
#include "Game/Game.h"
#include "Game/Resource.h"
#include "Game/Trade.h"

#define FIXME int

namespace k10engine {

namespace Game {

class Game;

class Player {
public:
    std::string id() { return m_id; }

    Flags get_flags();

    bool can_accept_trade() { return m_can_accept_trade; }
    void set_can_accept_trade(bool can_accept_trade) { m_can_accept_trade = can_accept_trade; }

    bool has_declined_trade() { return m_has_declined_trade; }
    void set_has_declined_trade(bool has_declined_trade)
    {
        m_has_declined_trade = has_declined_trade;
    }

    int num_to_discard() { return m_num_to_discard; }
    void set_num_to_discard(int num_to_discard) { m_num_to_discard = num_to_discard; }

    FIXME vertex() { return m_vertex; }
    void set_vertex(FIXME vertex) { m_vertex = vertex; }

    bool is_human() { return m_is_human; }

    bool has_heavy_purse();
    bool can_build_city();
    bool can_build_road();
    bool can_build_settlement();
    bool can_buy_development_card();
    bool can_play_knight();
    bool can_play_monopoly();
    bool can_play_road_building();
    bool can_play_victory_point();
    bool can_play_year_of_plenty();
    bool can_trade();
    bool can_trade_with_bank();
    bool is_current_player();
    bool is_blocking();

    bool can_afford(ResourceCounts);

    int army_size();
    int longest_road();

    int public_score();
    int private_score();

private:
    Player(std::string id, bool is_human, Game* game)
        : m_id(id)
        , m_is_human(is_human)
        , m_game(game)
    {
    }

    ~Player() {}

    Game* game() { return m_game; }

    std::string m_id;
    bool m_is_human;
    Game* m_game;

    ResourceCounts m_resources;
    FIXME m_played_development_cards;
    FIXME m_playable_development_cards;
    FIXME m_unplayable_development_cards;

    bool m_can_accept_trade{ false };
    bool m_has_declined_trade{ false };
    int m_num_to_discard{ 0 };
    FIXME m_vertex;

    std::vector<FIXME> m_cities;
    std::vector<FIXME> m_roads;
    std::vector<FIXME> m_settlements;

    int m_public_score;
    int m_private_score;
};

} // namespace Game

} // namespace k10engine
