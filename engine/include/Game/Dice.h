#pragma once

#include <cstddef>

namespace k10engine::Game {

#define k10_DICE_MIN 1
#define k10_DICE_MAX 6

#define k10_ROBBER_DICE_ROLL 7

class Dice {
public:
    Dice() {}
    void roll();
    size_t get_total() const;

private:
    size_t m_die_0{ 0 };
    size_t m_die_1{ 0 };
};

} // namespace k10engine::Game
