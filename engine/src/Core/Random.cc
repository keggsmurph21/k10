#include <algorithm>
#include <time.h>

#include "Core/Random.h"

namespace k10engine {

static Random* s_random;

Random* Random::the()
{
    if (s_random == nullptr) {
        s_random = new Random();
    }
    return s_random;
}

Random::Random()
    : m_rng(m_rd())
{
}

void Random::seed(int seed)
{
    the()->m_rng.seed(seed);
}

} // namespace k10engine
