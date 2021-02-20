#pragma once

#include "Util/Decoder.h"
#include "Util/Encoder.h"

namespace k10engine::Game {

#define k10_DICE_MIN 1
#define k10_DICE_MAX 6

#define k10_ROBBER_DICE_ROLL 7

#define k10_ENABLE_ROLL_DICE_EXACT

class Dice {
public:
    Dice() {}
    void roll();
    u8 get_total() const;
#ifdef k10_ENABLE_ROLL_DICE_EXACT
    void set_total(u8);
#endif

    u8 die_0() const { return m_die_0; }
    void set_die_0(u8 die) { m_die_0 = die; }

    u8 die_1() const { return m_die_1; }
    void set_die_1(u8 die) { m_die_1 = die; }

private:
    u8 m_die_0{ 0 };
    u8 m_die_1{ 0 };
};

} // namespace k10engine::Game

template<>
void encode(ByteBuffer&, const k10engine::Game::Dice&);

template<>
bool decode(ByteBuffer&, k10engine::Game::Dice&);
