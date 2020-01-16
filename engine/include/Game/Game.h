#pragma once

#include <random>
#include <vector>

#include "Game/Dice.h"
#include "Game/Player.h"
#include "Game/Trade.h"

namespace k10engine {

namespace Game {

std::random_device rd;                                    // initialize seed engine
std::mt19937 rng(rd());                                   // seed
std::uniform_int_distribution<int> random_selector(1, 6); // inclusive

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

private:
    bool m_can_steal{ false };
    bool m_has_rolled{ false };
    Dice m_dice;
    Trade m_current_trade{ nullptr };
    int m_turn{ 0 };
    std::vector<Player> m_players;
    Player m_current_player; // snakes at beginning
    Player m_has_largest_army;
    Player m_has_longest_road;
}

} // namespace Game

} // namespace k10engine
