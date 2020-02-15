#pragma once

#include <functional>
#include <optional>
#include <vector>

#include "Board/Graph.h"
#include "Core/Action.h"
#include "Core/Result.h"
#include "Core/Vertex.h"
#include "Game/BoardView/Hex.h"
#include "Game/BoardView/Junction.h"
#include "Game/BoardView/Road.h"
#include "Game/Dice.h"
#include "Game/Player.h"
#include "Game/Robber.h"
#include "Game/Trade.h"
#include "Scenario/Parameters.h"
#include "Scenario/Scenario.h"

namespace k10engine::Game {

class Player;

class Game {
public:
    bool can_steal() const { return m_can_steal; }
    void set_can_steal(bool can_steal) { m_can_steal = can_steal; }

    bool has_rolled() const { return m_has_rolled; }
    void set_has_rolled(bool has_rolled) { m_has_rolled = has_rolled; }

    bool is_game_over() const;

    bool is_trade_accepted() const { return m_is_trade_accepted; }
    void set_is_trade_accepted(bool is_trade_accepted) { m_is_trade_accepted = is_trade_accepted; }

    bool is_first_round() const;
    bool is_second_round() const;
    bool is_roll_seven() const;
    bool should_wait_for_discard() const;
    bool should_wait_for_trade() const;

    const BoardView::Hex* robber_location() const { return m_robber.location(); }

    void roll_dice();
    int get_dice_total() const;

    size_t turn() const { return m_turn; }
    void increment_turn();

    int get_round() const; // turn() // #players

    int largest_army() const;
    int longest_road() const;

    const std::map<size_t, BoardView::Hex*>& hexes() const { return m_hexes; }
    const std::map<size_t, BoardView::Junction*>& junctions() const { return m_junctions; }
    const std::map<size_t, BoardView::Road*>& roads() const { return m_roads; }

    const Player* current_player() const { return m_players.at(m_current_player_index); }
    const std::vector<Player*>& players() const { return m_players; }

    Result execute_action(size_t player_id, const Action&);

    Game(const Board::Graph*,
         std::map<size_t, BoardView::Hex*>,
         std::map<size_t, BoardView::Junction*>,
         std::map<size_t, BoardView::Road*>,
         std::vector<DevelopmentCard>,
         const Scenario::Scenario&,
         const Scenario::Parameters&,
         int robber_index);
    ~Game();

private:
    Player* current_player() { return m_players.at(m_current_player_index); }

    const Board::Graph* m_graph;

    std::map<size_t, BoardView::Hex*> m_hexes;
    std::map<size_t, BoardView::Junction*> m_junctions;
    std::map<size_t, BoardView::Road*> m_roads;
    std::vector<DevelopmentCard> m_deck;

    const Scenario::Scenario& m_scenario;

    Dice m_dice;
    Robber m_robber;

    size_t m_victory_points_goal;

    std::vector<Player*> m_players;

    size_t m_deck_index{ 0 };
    size_t m_current_player_index{ 0 };

    bool m_can_steal{ false };
    bool m_has_rolled{ false };
    bool m_is_trade_accepted{ false };
    std::optional<Trade> m_current_trade;
    size_t m_turn{ 0 };

    Player* m_has_largest_army{ nullptr };
    Player* m_has_longest_road{ nullptr };
};

Game* initialize(const Board::Graph*, const Scenario::Scenario&, const Scenario::Parameters&);

} // namespace k10engine::Game
