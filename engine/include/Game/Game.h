#pragma once

#include <list>
#include <vector>

#include "Board/Graph.h"
#include "Game/BoardView/Hex.h"
#include "Game/BoardView/Junction.h"
#include "Game/BoardView/Road.h"
#include "Game/Dice.h"
#include "Game/Player.h"
#include "Game/Robber.h"
#include "Game/Trade.h"
#include "Scenario/Parameters.h"
#include "Scenario/Scenario.h"

namespace k10engine {

namespace Game {

class Player;

class Game {
public:
    bool can_steal() { return m_can_steal; }
    void set_can_steal(bool can_steal) { m_can_steal = can_steal; }

    bool has_rolled() { return m_has_rolled; }
    void set_has_rolled(bool has_rolled) { m_has_rolled = has_rolled; }

    bool is_game_over() { return m_is_game_over; }
    void set_is_game_over(bool is_game_over) { m_is_game_over = is_game_over; }

    bool is_trade_accepted() { return m_is_trade_accepted; }
    void set_is_trade_accepted(bool is_trade_accepted) { m_is_trade_accepted = is_trade_accepted; }

    bool is_first_round();
    bool is_second_round();
    bool is_roll_seven();
    bool should_wait_for_discard();
    bool should_wait_for_trade();

    void roll_dice();
    int get_dice_total();

    int turn() { return m_turn; }
    void increment_turn() { ++m_turn; }

    int get_round(); // turn() // #players

    int largest_army();
    int longest_road();

    static Game* initialize(Board::Graph*, Scenario::Scenario*, Scenario::Parameters*);
    ~Game() = default;

private:
    Game();

    std::vector<BoardView::Hex> m_hexes;
    std::vector<BoardView::Junction> m_junctions;
    std::vector<BoardView::Road> m_roads;
    std::list<DevelopmentCard> m_deck;

    Scenario::Scenario m_scenario;

    int m_victory_points_goal;
    bool m_can_steal;
    bool m_has_rolled;
    bool m_is_game_over;
    bool m_is_trade_accepted;
    Dice m_dice;
    Robber m_robber;
    Trade m_current_trade;
    int m_turn;
    std::vector<Player*> m_players;
    Player* m_current_player; // snakes at beginning
    Player* m_has_largest_army;
    Player* m_has_longest_road;
};

} // namespace Game

} // namespace k10engine
