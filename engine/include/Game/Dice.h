#pragma once

namespace k10engine::Game {

#define k10_DICE_MIN 1
#define k10_DICE_MAX 6

class Dice {
public:
    Dice() {}
    void roll();
    int get_total();

private:
    int m_die_0{ 0 };
    int m_die_1{ 0 };
};

} // namespace k10engine::Game
