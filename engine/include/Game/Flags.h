#pragma once

#include "Core/Vertex.h"

namespace k10engine {

namespace Game {

struct BuildFlags {
    bool city;
    bool development_card;
    bool road;
    bool settlement;
};

struct DevelopmentCardFlags {
    bool knight;
    bool monopoly;
    bool road_building;
    bool victory_point;
    bool year_of_plenty;
};

struct Flags {
    State::Vertex vertex;
    bool can_accept_trade;
    BuildFlags can_build;
    DevelopmentCardFlags can_play;
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
};

} // namespace Game

} // namespace k10engine
