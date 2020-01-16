#pragma once

namespace k10engine {

namespace Game {

typedef struct {
    bool can_accept_trade;
    bool can_build_city;
    bool can_build_road;
    bool can_build_settlement;
    bool can_buy_development_card;
    bool can_play_knight;
    bool can_play_monopoly;
    bool can_play_road_building;
    bool can_play_victory_point;
    bool can_play_year_of_plenty;
    bool can_steal;
    bool can_trade;
    bool can_trade_with_bank;
    bool has_rolled;
    bool is_current_player;
    bool is_first_round;
    bool is_game_over;
    bool is_roll_seven;
    bool is_second_round;
    bool is_trade_accepted;
    bool should_wait_for_discard;
    bool should_wait_for_trade;
    int num_to_discard;
} Flags;

} // namespace Game

} // namespace k10engine
