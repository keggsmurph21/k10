#include <time.h>

#include "Core/Random.h"

namespace k10engine {

Random* Random::the()
{
    if (!s_singleton)
        s_singleton = new Random();
    return s_singleton;
}

int Random::get_dice_roll()
{
    return the()->dice_dis();
}

Random::Random()
    : m_rng(m_rd())
    , m_dice_dis(std::uniform_int_distribution<>(1, 6))
{
}

void Random::set_seed(int seed)
{
    m_rng.seed(seed);
}

void Random::seed(int seed)
{
    the()->set_seed(seed);
}

void Random::seed()
{
    the()->set_seed(time(0));
}

} // namespace k10engine
