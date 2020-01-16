#include <time.h>

#include "Game/Dice.h"

namespace k10engine {

namespace Game {

Dice::Dice()
    : m_die_0(0)
    , m_die_1(0)
    , m_rng(m_rd())
    , m_dice_dis(std::uniform_int_distribution<>(1, 6))
{
    m_rng.seed(::time(nullptr));
}

void Dice::roll()
{
    m_die_0 = m_dice_dis(m_rng);
    m_die_1 = m_dice_dis(m_rng);
}

int Dice::get_total()
{
    return m_die_0 + m_die_1;
}

} // namespace Game

} // namespace k10engine
