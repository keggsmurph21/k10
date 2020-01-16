#include <assert.h>

#include "Game/Game.h"

namespace k10engine {

namespace Game {

void Game::roll_dice()
{
    assert(false);
}

int Game::get_dice_total()
{
    return m_dice.get_total();
}

bool Game::is_first_round()
{
    return get_round() == 0;
}

bool Game::is_second_round()
{
    return get_round() == 1;
}

bool Game::is_roll_seven()
{
    return get_dice_total() == 7;
}

bool Game::should_wait_for_discard()
{
    assert(false);
}

bool Game::should_wait_for_trade()
{
    assert(false);
}

int Game::get_round()
{
    assert(false);
}

} // namespace Game

} // namespace k10engine
