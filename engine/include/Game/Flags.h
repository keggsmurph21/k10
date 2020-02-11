#pragma once

#include <set>

#include "Core/Building.h"
#include "Core/DevelopmentCard.h"
#include "Core/Vertex.h"

namespace k10engine {

namespace Game {

struct Flags {
    State::Vertex vertex;
    bool can_accept_trade;
    std::set<Building> buildable_buildings;
    std::set<DevelopmentCard> playable_development_cards;
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
