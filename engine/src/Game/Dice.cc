#include <cassert>

#include "Core/Random.h"
#include "Game/Dice.h"

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

#ifdef k10_ENABLE_ROLL_DICE_EXACT
void Dice::set_total(size_t total)
{
    assert(1 < total && total < 13);
    if (total > 6) {
        m_die_0 = 6;
        m_die_1 = total - 6;
    } else {
        m_die_0 = 1;
        m_die_1 = total - 1;
    }
}
#endif

} // namespace k10engine::Game
