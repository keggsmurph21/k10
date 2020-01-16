#pragma once

namespace k10engine {

namespace Game {

class Dice {
public:
    void roll();
    int get_total();

private:
    int m_die_0{ 0 };
    int m_die_1{ 0 };
};

} // namespace Game

} // namespace k10engine
