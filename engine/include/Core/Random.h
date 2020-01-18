#pragma once

#include <random>
#include <vector>

namespace k10engine {

class Random {
public:
    static Random* the();
    static void seed(int);

    typedef std::mt19937 MersenneTwister;
    static MersenneTwister rng() { return the()->m_rng; }

private:
    Random();
    std::random_device m_rd;
    MersenneTwister m_rng;
};

} // namespace k10engine
