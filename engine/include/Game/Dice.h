#pragma once

#include <random>

namespace k10engine {

namespace Game {

class Dice {
public:
    Dice();
    void roll();
    int get_total();

private:
    int m_die_0{ 0 };
    int m_die_1{ 0 };

    // random number generator initialization
    std::random_device m_rd;
    typedef std::mt19937 rng_t;
    rng_t m_rng;
    std::uniform_int_distribution<int> m_dice_dis;
};

} // namespace Game

} // namespace k10engine
