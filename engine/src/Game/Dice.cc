#include "Game/Dice.h"
#include "Core/Random.h"

static auto s_dice_dist = std::uniform_int_distribution<size_t>(k10_DICE_MIN, k10_DICE_MAX);

namespace k10engine::Game {

void Dice::roll()
{
    auto rng = Random::rng();
    m_die_0 = s_dice_dist(rng);
    m_die_1 = s_dice_dist(rng);
}

size_t Dice::get_total() const
{
    return m_die_0 + m_die_1;
}

} // namespace k10engine::Game
