#pragma once

#include <random>

namespace k10engine {

class Random {
public:
    static void seed();
    static void seed(int);
    static int get_dice_roll();

private:
    static inline Random* s_singleton{ nullptr };

    static Random* the();
    void set_seed(int);
    Random();

    std::random_device m_rd;
    typedef std::mt19937 MersenneTwister;
    MersenneTwister m_rng;

    std::uniform_int_distribution<int> m_dice_dis;
    int dice_dis() { return m_dice_dis(m_rng); }
};

} // namespace k10engine
