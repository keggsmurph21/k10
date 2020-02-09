#include <stdexcept>

#include "Game/Player.h"

namespace k10engine::Game {

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

bool Player::has_heavy_purse() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::has_heavy_purse");
}

bool Player::can_build_city() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::can_build_city");
}

bool Player::can_build_road() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::can_build_road");
}

bool Player::can_build_settlement() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::can_build_settlement");
}

bool Player::can_buy_development_card() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::can_buy_development_card");
}

bool Player::can_play_knight() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::can_play_knight");
}

bool Player::can_play_monopoly() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::can_play_monopoly");
}

bool Player::can_play_road_building() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::can_play_road_building");
}

bool Player::can_play_victory_point() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::can_play_victory_point");
}

bool Player::can_play_year_of_plenty() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::can_play_year_of_plenty");
}

bool Player::can_trade() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::can_trade");
}

bool Player::can_trade_with_bank() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::can_trade_with_bank");
}

bool Player::is_current_player() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::is_current_player");
}

bool Player::is_blocking() // NOLINT(readability-convert-member-functions-to-static)
{
    throw std::invalid_argument("Not implemented: Player::is_blocking");
}

bool Player::can_afford( // NOLINT(readability-convert-member-functions-to-static)
    const ResourceCounts& resource_counts)
{
    (void)resource_counts;
    throw std::invalid_argument("Not implemented: Player::can_afford");
}

} // namespace k10engine::Game
