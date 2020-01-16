#include <assert.h>

#include "Game/Player.h"

namespace k10engine {

namespace Game {

Flags Player::get_flags()
{
    Flags flags;
    flags.can_accept_trade = game()->is_game_over();
    flags.can_build_city = can_build_city();
    flags.can_build_road = can_build_road();
    flags.can_build_settlement = can_build_settlement();
    flags.can_buy_development_card = can_buy_development_card();
    flags.can_play_knight = can_play_knight();
    flags.can_play_monopoly = can_play_monopoly();
    flags.can_play_road_building = can_play_road_building();
    flags.can_play_victory_point = can_play_victory_point();
    flags.can_play_year_of_plenty = can_play_year_of_plenty();
    flags.can_build_road = can_build_road();
    flags.can_steal = game()->can_steal();
    flags.can_trade = can_trade();
    flags.can_trade_with_bank = can_trade_with_bank();
    flags.has_rolled = game()->has_rolled();
    flags.is_current_player = is_current_player();
    flags.is_first_round = game()->is_first_round();
    flags.is_game_over = game()->is_game_over();
    flags.is_roll_seven = game()->is_roll_seven();
    flags.is_second_round = game()->is_second_round();
    flags.is_trade_accepted = game()->is_trade_accepted();
    flags.should_wait_for_discard = game()->should_wait_for_discard();
    flags.should_wait_for_trade = game()->should_wait_for_trade();
    flags.num_to_discard = num_to_discard();
    return flags;
}

bool Player::has_heavy_purse()
{
    assert(false);
}

bool Player::can_build_city()
{
    assert(false);
}

bool Player::can_build_road()
{
    assert(false);
}

bool Player::can_build_settlement()
{
    assert(false);
}

bool Player::can_buy_development_card()
{
    assert(false);
}

bool Player::can_play_knight()
{
    assert(false);
}

bool Player::can_play_monopoly()
{
    assert(false);
}

bool Player::can_play_road_building()
{
    assert(false);
}

bool Player::can_play_victory_point()
{
    assert(false);
}

bool Player::can_play_year_of_plenty()
{
    assert(false);
}

bool Player::can_trade()
{
    assert(false);
}

bool Player::can_trade_with_bank()
{
    assert(false);
}

bool Player::is_current_player()
{
    assert(false);
}

bool Player::is_blocking()
{
    assert(false);
}

bool Player::can_afford(ResourceCounts)
{
    assert(false);
}

} // namespace Game

} // namespace k10engine
