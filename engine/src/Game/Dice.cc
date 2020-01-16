#include <time.h>

#include "Core/Random.h"
#include "Game/Dice.h"

namespace k10engine {

namespace Game {

Dice::Dice()
    : m_die_0(0)
    , m_die_1(0)
{
}

void Dice::roll()
{
    m_die_0 = Random::get_dice_roll();
    m_die_1 = Random::get_dice_roll();
}

int Dice::get_total()
{
    return m_die_0 + m_die_1;
}

} // namespace Game

} // namespace k10engine
